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
	idle->m_This=idle;
	FlagHelper::Set(idle->m_Flags, TaskFlags::Suspended);
	s_IdleTask[core]=idle;
	s_CurrentTask[core]=idle;
	}
auto main=Task::CreateInternal(MainTask, "main");
main->m_This=main;
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


//==================
// Common Protected
//==================

VOID Scheduler::AddTask(Task* task)
{
task->m_This=task;
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Owner);
task->m_Owner=current;
AddCreateTask(&s_Create, task);
}

VOID Scheduler::CancelTask(Task* task)
{
SpinLock lock(s_CriticalSection);
BOOL resume=false;
if(task->m_ResumeTime)
	{
	RemoveSleepingTask(&s_Sleeping, task);
	task->m_ResumeTime=0;
	resume=true;
	}
auto signal=task->m_Signal;
if(signal)
	{
	RemoveParallelTask(&signal->m_Waiting, task);
	task->m_Signal=nullptr;
	resume=true;
	}
if(resume)
	ResumeTask(task, Status::Aborted);
}

VOID Scheduler::ExitTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Release);
SuspendCurrentTask(nullptr, core, current);
assert(0);
}

Task* Scheduler::GetCurrentTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
return s_CurrentTask[core];
}

VOID Scheduler::ReleaseTasks()
{
SpinLock lock(s_CriticalSection);
auto release=s_Release;
s_Release=nullptr;
lock.Unlock();
while(release)
	{
	auto next=release->m_Release;
	release->m_Release=nullptr;
	release->m_This=nullptr;
	release=next;
	}
}

VOID Scheduler::Schedule()
{
ReleaseTasks();
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
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
current->m_ResumeTime=resume_time;
AddSleepingTask(&s_Sleeping, current);
SuspendCurrentTask(nullptr, core, current);
lock.Yield();
if(current->Cancelled)
	throw AbortException();
}

VOID Scheduler::WakeupTasks()
{
SpinLock lock(s_CriticalSection);
auto current_ptr=&s_Sleeping;
UINT64 time=SystemTimer::GetTickCount64();
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current->m_ResumeTime>time)
		break;
	auto next=current->m_Sleeping;
	current->m_Sleeping=nullptr;
	current->m_ResumeTime=0;
	AddWaitingTask(current);
	*current_ptr=next;
	}
}


//================
// Common Private
//================

VOID Scheduler::AddCreateTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Create;
	}
*current_ptr=task;
}

VOID Scheduler::AddParallelTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Parallel;
	}
*current_ptr=task;
}

VOID Scheduler::AddReleaseTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Release;
	}
*current_ptr=task;
}

VOID Scheduler::AddSleepingTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current->m_ResumeTime>task->m_ResumeTime)
		{
		*current_ptr=task;
		task->m_Sleeping=current;
		return;
		}
	current_ptr=&current->m_Sleeping;
	}
*current_ptr=task;
}

VOID Scheduler::AddWaitingTask(Task* task)
{
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	if(!s_WaitingFirst)
		{
		s_WaitingFirst=task;
		s_WaitingLast=task;
		s_WaitingLocked=task;
		return;
		}
	if(!s_WaitingLocked)
		{
		task->m_Waiting=s_WaitingFirst;
		s_WaitingFirst=task;
		s_WaitingLocked=task;
		return;
		}
	if(!s_WaitingLocked->m_Waiting)
		{
		s_WaitingLocked->m_Waiting=task;
		s_WaitingLast=task;
		s_WaitingLocked=task;
		return;
		}
	task->m_Waiting=s_WaitingLocked->m_Waiting;
	s_WaitingLocked=task;
	return;
	}
if(!s_WaitingFirst)
	{
	s_WaitingFirst=task;
	s_WaitingLast=task;
	return;
	}
s_WaitingLast->m_Waiting=task;
s_WaitingLast=task;
}

VOID Scheduler::AddWaitingTask(Task** current_ptr, Task* task)
{
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			task->m_Waiting=current;
			*current_ptr=task;
			return;
			}
		current_ptr=&current->m_Waiting;
		}
	}
else
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		current_ptr=&current->m_Waiting;
		}
	}
*current_ptr=task;
}

VOID Scheduler::CreateTasks()
{
auto current_ptr=&s_Create;
while(*current_ptr)
	{
	auto current=*current_ptr;
	auto owner=current->m_Owner;
	if(FlagHelper::Get(owner->m_Flags, TaskFlags::Owner))
		{
		current_ptr=&current->m_Create;
		continue;
		}
	auto next=current->m_Create;
	current->m_Owner=nullptr;
	current->m_Create=nullptr;
	AddWaitingTask(current);
	*current_ptr=next;
	}
}

UINT Scheduler::GetCurrentCore()
{
s_CurrentCore=(s_CurrentCore+1)%s_CoreCount;
return s_CurrentCore;
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

Task* Scheduler::GetWaitingTask()
{
auto resume=s_WaitingFirst;
if(!resume)
	return nullptr;
auto waiting=resume->m_Waiting;
resume->m_Waiting=nullptr;
s_WaitingFirst=waiting;
if(!waiting)
	s_WaitingLast=nullptr;
if(resume==s_WaitingLocked)
	s_WaitingLocked=nullptr;
return resume;
}

VOID Scheduler::HandleTaskSwitch(VOID* param)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Switch);
auto next=current->m_Next;
current->m_Next=nullptr;
if(FlagHelper::Get(current->m_Flags, TaskFlags::Suspended))
	{
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Release))
		AddReleaseTask(&s_Release, current);
	}
else
	{
	AddWaitingTask(current);
	}
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

VOID Scheduler::RemoveParallelTask(Task** current_ptr, Task* task)
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

VOID Scheduler::RemoveSleepingTask(Task** current_ptr, Task* task)
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
	assert(FlagHelper::Get(resume->m_Flags, TaskFlags::Suspended));
	FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
	auto next=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	resume->m_Status=status;
	if(GetNextCore(&core))
		{
		auto current=s_CurrentTask[core];
		FlagHelper::Set(current->m_Flags, TaskFlags::Switch);
		current->m_Next=resume;
		Interrupts::Send(IRQ_TASK_SWITCH, core);
		}
	else
		{
		AddWaitingTask(resume);
		}
	resume=next;
	}
}

VOID Scheduler::SuspendCurrentTask(Task** owner_ptr, UINT core, Task* current)
{
if(FlagHelper::Get(current->m_Flags, TaskFlags::Owner))
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Owner);
	CreateTasks();
	}
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

UINT Scheduler::s_CoreCount=0;
Task* Scheduler::s_Create=nullptr;
CriticalSection Scheduler::s_CriticalSection;
UINT Scheduler::s_CurrentCore=0;
Task* Scheduler::s_CurrentTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_IdleTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_MainTask=nullptr;
Task* Scheduler::s_Release=nullptr;
Task* Scheduler::s_Sleeping=nullptr;
Task* Scheduler::s_WaitingFirst=nullptr;
Task* Scheduler::s_WaitingLast=nullptr;
Task* Scheduler::s_WaitingLocked=nullptr;

}