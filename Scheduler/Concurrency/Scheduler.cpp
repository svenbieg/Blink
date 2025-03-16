//===============
// Scheduler.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Concurrency/TaskHelper.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"
#include "FlagHelper.h"

using namespace Devices::System;
using namespace Devices::Timers;

extern INT Main();


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID Scheduler::Begin()
{
SpinLock lock(s_CriticalSection);
UINT core_count=++s_CoreCount;
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
Interrupts::Enable();
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}

VOID Scheduler::Initialize()
{
Interrupts::Route(IRQ_TASK_SWITCH, IrqTarget::All);
Interrupts::SetHandler(IRQ_TASK_SWITCH, HandleTaskSwitch);
for(UINT core=0; core<CPU_COUNT; core++)
	{
	auto idle=Task::CreateInternal(IdleTask, String::Create("idle%u", core));
	FlagHelper::Set(idle->m_Flags, TaskFlags::Suspended);
	s_IdleTask[core]=idle;
	s_CurrentTask[core]=idle;
	}
auto main=Task::CreateInternal(MainTask, "main");
s_CurrentTask[0]=main;
s_MainTask=main;
}

BOOL Scheduler::IsMainTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
return current==s_MainTask;
}


//================
// Common Private
//================

VOID Scheduler::AddParallelTask(Handle<Task>* current_ptr, Task* parallel)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Parallel;
	}
*current_ptr=parallel;
}

VOID Scheduler::AddSleepingTask(Task* task, UINT64 resume_time)
{
TaskLock lock(s_SleepingMutex);
Handle<Task>& sleeping=s_SleepingTasks.get(resume_time);
AddSleepingTask(&sleeping, task);
task->m_ResumeTime=resume_time;
}

VOID Scheduler::AddSleepingTask(Handle<Task>* current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Sleeping;
	}
*current_ptr=task;
}

VOID Scheduler::AddTask(Task* task)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Owner);
task->m_Owner=current;
AddWaitingTask(&s_WaitingTask, task);
}

VOID Scheduler::AddWaitingTask(Handle<Task>* current_ptr, Task* waiting)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=waiting);
	if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Locked))
		{
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			waiting->m_Waiting=current;
			*current_ptr=waiting;
			return;
			}
		}
	current_ptr=&current->m_Waiting;
	}
*current_ptr=waiting;
}

VOID Scheduler::AddWakeupTask(Handle<Task>* current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Wakeup;
	}
*current_ptr=task;
}

VOID Scheduler::CancelTask(Task* task)
{
SpinLock lock(s_CriticalSection);
BOOL resume=false;
if(task->m_ResumeTime)
	resume=true;
auto signal=task->m_Signal;
if(signal)
	{
	signal->RemoveWaitingTask(task);
	task->m_Signal=nullptr;
	resume=true;
	}
if(resume)
	ResumeTask(task, Status::Aborted);
}

VOID Scheduler::ExitTask()
{
SuspendCurrentTask(nullptr);
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

UINT Scheduler::GetCurrentCore()
{
s_CurrentCore=(s_CurrentCore+1)%s_CoreCount;
return s_CurrentCore;
}

Handle<Task> Scheduler::GetCurrentTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
return s_CurrentTask[core];
}

BOOL Scheduler::GetNextCore(UINT* core_ptr)
{
UINT last_id=s_CurrentCore+s_CoreCount;
for(UINT id=*core_ptr; id<last_id; id++)
	{
	UINT core=id%s_CoreCount;
	auto current=s_CurrentTask[core];
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Busy))
		continue;
	*core_ptr=core;
	return true;
	}
return false;
}

Handle<Task> Scheduler::GetWaitingTask()
{
if(!s_WaitingTask)
	return nullptr;
auto current_ptr=&s_WaitingTask;
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current->m_Owner)
		{
		auto owner=current->m_Owner;
		if(FlagHelper::Get(owner->m_Flags, TaskFlags::Owner))
			{
			current_ptr=&current->m_Waiting;
			continue;
			}
		current->m_Owner=nullptr;
		}
	auto waiting=current->m_Waiting;
	current->m_Waiting=nullptr;
	*current_ptr=waiting;
	return current;
	}
return nullptr;
}

VOID Scheduler::HandleTaskSwitch(VOID* param)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Switch);
auto next=current->m_Next;
current->m_Next=nullptr;
if(!FlagHelper::Get(current->m_Flags, TaskFlags::Suspended))
	AddWaitingTask(&s_WaitingTask, current);
s_CurrentTask[core]=next;
TaskHelper::Switch(core, current, next);
}

VOID Scheduler::IdleTask()
{
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

VOID Scheduler::MainTask()
{
auto timer=SystemTimer::Get();
timer->Triggered.Add(Scheduler::Schedule);
auto status=Status::Success;
try
	{
	Main();
	}
catch(...)
	{
	status=Status::Error;
	}
System::Restart();
}

VOID Scheduler::RemoveParallelTask(Handle<Task>* current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current==task)
		{
		auto parallel=current->m_Parallel;
		current->m_Parallel=nullptr;
		parallel->m_Waiting=current->m_Waiting;
		current->m_Waiting=nullptr;
		*current_ptr=parallel;
		break;
		}
	current_ptr=&current->m_Parallel;
	}
}

VOID Scheduler::RemoveSleepingTask(Handle<Task>* current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current==task)
		{
		auto sleeping=current->m_Sleeping;
		current->m_Sleeping=nullptr;
		*current_ptr=sleeping;
		break;
		}
	current_ptr=&current->m_Sleeping;
	}
}

VOID Scheduler::ResumeTask(Task* resume, Status status)
{
UINT core=GetCurrentCore();
while(resume)
	{
	resume->m_Status=status;
	if(resume->m_ResumeTime)
		AddWakeupTask(&s_WakeupTask, resume);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	if(FlagHelper::Get(resume->m_Flags, TaskFlags::Suspended))
		{
		FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
		if(GetNextCore(&core))
			{
			auto current=s_CurrentTask[core];
			FlagHelper::Set(current->m_Flags, TaskFlags::Switch);
			current->m_Next=resume;
			Interrupts::Send(IRQ_TASK_SWITCH, core);
			}
		else
			{
			AddWaitingTask(&s_WaitingTask, resume);
			}
		}
	resume=parallel;
	}
}

VOID Scheduler::Schedule()
{
WakeupTasks();
SpinLock lock(s_CriticalSection);
UINT core=GetCurrentCore();
while(GetNextCore(&core))
	{
	auto next=GetWaitingTask();
	if(!next)
		break;
	auto current=s_CurrentTask[core];
	FlagHelper::Set(current->m_Flags, TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
UINT64 resume_time=SystemTimer::GetTickCount64()+ms;
auto current=GetCurrentTask();
AddSleepingTask(current, resume_time);
SpinLock lock(s_CriticalSection);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
FlagHelper::Clear(current->m_Flags, TaskFlags::Owner);
UINT core=Cpu::GetId();
if(!current->m_Next)
	{
	auto next=GetWaitingTask();
	if(!next)
		next=s_IdleTask[core];
	FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
	FlagHelper::Set(current->m_Flags, TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
lock.Yield();
if(current->Cancelled)
	throw AbortException();
}

VOID Scheduler::SuspendCurrentTask(Handle<Task>* owner_ptr)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Owner);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
if(!current->m_Next)
	{
	auto next=GetWaitingTask();
	if(!next)
		next=s_IdleTask[core];
	FlagHelper::Set(current->m_Flags, TaskFlags::Switch);
	current->m_Next=next;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
if(owner_ptr)
	AddWaitingTask(owner_ptr, current);
}

VOID Scheduler::WakeupTasks()
{
TaskLock sleep_lock(s_SleepingMutex);
SpinLock lock(s_CriticalSection);
auto wakeup=s_WakeupTask;
s_WakeupTask=nullptr;
lock.Unlock();
while(wakeup)
	{
	auto next=wakeup->m_Wakeup;
	wakeup->m_Wakeup=nullptr;
	UINT64 resume_time=wakeup->m_ResumeTime;
	wakeup->m_ResumeTime=0;
	Handle<Task>& sleeping=s_SleepingTasks.get(resume_time);
	RemoveSleepingTask(&sleeping, wakeup);
	if(!sleeping)
		s_SleepingTasks.remove(resume_time);
	wakeup=next;
	}
UINT64 time=SystemTimer::GetTickCount64();
for(auto it=s_SleepingTasks.begin(); it.has_current(); )
	{
	UINT64 resume_time=it.get_key();
	if(resume_time>time)
		break;
	auto resume=it.get_value();
	it.remove_current();
	AddWakeupTask(&wakeup, resume);
	}
sleep_lock.Unlock();
lock.Lock();
while(wakeup)
	{
	auto next=wakeup->m_Wakeup;
	wakeup->m_Wakeup=nullptr;
	AddWaitingTask(&s_WaitingTask, wakeup);
	wakeup=next;
	}
}

UINT Scheduler::s_CoreCount=0;
CriticalSection Scheduler::s_CriticalSection;
UINT Scheduler::s_CurrentCore=0;
Handle<Task> Scheduler::s_CurrentTask[CPU_COUNT];
Handle<Task> Scheduler::s_IdleTask[CPU_COUNT];
Task* Scheduler::s_MainTask=nullptr;
Mutex Scheduler::s_SleepingMutex;
Collections::map<UINT64, Handle<Task>> Scheduler::s_SleepingTasks;
Handle<Task> Scheduler::s_WaitingTask;
Handle<Task> Scheduler::s_WakeupTask;

}