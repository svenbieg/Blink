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
	FlagHelper::Set(idle->m_Flags, TaskFlags::Idle);
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
SuspendCurrentTask(core, current);
lock.Unlock();
while(1)
	Cpu::WaitForInterrupt();
}

Task* Scheduler::GetCurrentTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
return s_CurrentTask[core];
}

VOID Scheduler::Schedule()
{
SpinLock lock(s_CriticalSection);
if(s_Release)
	{
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
	lock.Lock();
	}
if(s_Sleeping)
	{
	UINT64 time=SystemTimer::GetTickCount64();
	auto current_ptr=&s_Sleeping;
	while(*current_ptr)
		{
		auto current=*current_ptr;
		if(current->m_ResumeTime>time)
			break;
		auto next=current->m_Sleeping;
		current->m_Sleeping=nullptr;
		FlagHelper::Clear(current->m_Flags, TaskFlags::Suspended);
		AddWaitingTask(current);
		*current_ptr=next;
		}
	}
UINT waiting_count=GetWaitingCount(s_WaitingFirst, s_CoreCount);
if(!waiting_count)
	return;
UINT cores[CPU_COUNT];
UINT core_count=GetAvailableCores(cores, waiting_count);
for(UINT core_id=0; core_id<core_count; core_id++)
	{
	UINT core=cores[core_id];
	auto current=s_CurrentTask[core];
	auto resume=GetWaitingTask();
	current->m_Next=resume;
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
UINT64 resume_time=SystemTimer::GetTickCount64()+ms;
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
SuspendCurrentTask(core, current, resume_time);
lock.Yield();
if(current->Cancelled)
	throw AbortException();
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

VOID Scheduler::AddWaitingTask(Task* task, BOOL prepend)
{
assert(!FlagHelper::Get(task->m_Flags, TaskFlags::Suspended));
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
	task->m_Waiting=s_WaitingLocked->m_Waiting;
	s_WaitingLocked->m_Waiting=task;
	if(!task->m_Waiting)
		s_WaitingLast=task;
	return;
	}
if(!s_WaitingFirst)
	{
	s_WaitingFirst=task;
	s_WaitingLast=task;
	return;
	}
if(prepend)
	{
	if(s_WaitingLocked)
		{
		task->m_Waiting=s_WaitingLocked->m_Waiting;
		s_WaitingLocked->m_Waiting=task;
		if(!task->m_Waiting)
			s_WaitingLast=task;
		return;
		}
	task->m_Waiting=s_WaitingFirst;
	s_WaitingFirst=task;
	return;
	}
s_WaitingLast->m_Waiting=task;
s_WaitingLast=task;
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

UINT Scheduler::GetAvailableCores(UINT* cores, UINT max)
{
UINT count=0;
UINT mask=0;
static_assert(CPU_COUNT<=32);
for(UINT core=0; core<s_CoreCount; core++)
	{
	auto current=s_CurrentTask[core];
	BOOL idle=FlagHelper::Get(current->m_Flags, TaskFlags::Idle);
	auto next=current->m_Next;
	if(next)
		idle|=FlagHelper::Get(next->m_Flags, TaskFlags::Idle);
	if(!idle)
		continue;
	cores[count++]=core;
	if(count==max)
		return count;
	mask|=(1UL<<core);
	}
for(UINT core=0; core<s_CoreCount; core++)
	{
	if(mask&(1UL<<core))
		continue;
	auto current=s_CurrentTask[core];
	if(current->m_Next)
		continue;
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		continue;
	cores[count++]=core;
	if(count==max)
		break;
	}
return count;
}

UINT Scheduler::GetParallelCount(Task* parallel, UINT max)
{
UINT count=0;
while(parallel)
	{
	parallel=parallel->m_Parallel;
	if(++count==max)
		break;
	}
return count;
}

UINT Scheduler::GetWaitingCount(Task* waiting, UINT max)
{
UINT count=0;
while(waiting)
	{
	waiting=waiting->m_Waiting;
	if(++count==max)
		break;
	}
return count;
}

Task* Scheduler::GetWaitingTask()
{
auto resume=s_WaitingFirst;
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
auto next=current->m_Next;
assert(next);
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

BOOL Scheduler::RemoveParallelTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	if(current==task)
		{
		auto parallel=current->m_Parallel;
		if(parallel)
			{
			parallel->m_Waiting=current->m_Waiting;
			current->m_Parallel=nullptr;
			current->m_Waiting=nullptr;
			}
		*current_ptr=parallel;
		return true;
		}
	current_ptr=&current->m_Parallel;
	}
return false;
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
while(resume)
	{
	FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
	if(resume->m_ResumeTime)
		{
		RemoveSleepingTask(&Scheduler::s_Sleeping, resume);
		resume->m_ResumeTime=0;
		}
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	resume->m_Status=status;
	AddWaitingTask(resume, true);
	resume=parallel;
	}
SwitchCurrentTask();
}

VOID Scheduler::ResumeTask(UINT core, Task* current, Task* resume)
{
while(resume)
	{
	FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
	auto parallel=resume->m_Parallel;
	resume->m_Parallel=nullptr;
	AddWaitingTask(resume, true);
	resume=parallel;
	}
SwitchCurrentTask(core, current);
}

VOID Scheduler::SuspendCurrentTask(UINT core, Task* current, UINT64 resume_time)
{
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Suspended));
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
if(FlagHelper::Get(current->m_Flags, TaskFlags::Owner))
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Owner);
	CreateTasks();
	}
if(resume_time)
	{
	current->m_ResumeTime=resume_time;
	AddSleepingTask(&s_Sleeping, current);
	}
if(current->m_Next)
	return;
auto next=GetWaitingTask();
if(!next)
	next=s_IdleTask[core];
current->m_Next=next;
Interrupts::Send(IRQ_TASK_SWITCH, core);
}

VOID Scheduler::SwitchCurrentTask()
{
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
SwitchCurrentTask(core, current);
}

VOID Scheduler::SwitchCurrentTask(UINT core, Task* current)
{
if(!s_WaitingFirst)
	return;
auto next=current->m_Next;
if(!next)
	{
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		return;
	current->m_Next=GetWaitingTask();
	Interrupts::Send(IRQ_TASK_SWITCH, core);
	return;
	}
if(FlagHelper::Get(next->m_Flags, TaskFlags::Idle))
	{
	current->m_Next=GetWaitingTask();
	return;
	}
if(!s_WaitingLocked||FlagHelper::Get(next->m_Flags, TaskFlags::Locked))
	return;
auto waiting=GetWaitingTask();
AddWaitingTask(next, true);
current->m_Next=waiting;
}

UINT Scheduler::s_CoreCount=0;
Task* Scheduler::s_Create=nullptr;
CriticalSection Scheduler::s_CriticalSection;
Task* Scheduler::s_CurrentTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_IdleTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_MainTask=nullptr;
Task* Scheduler::s_Release=nullptr;
Task* Scheduler::s_Sleeping=nullptr;
Task* Scheduler::s_WaitingFirst=nullptr;
Task* Scheduler::s_WaitingLast=nullptr;
Task* Scheduler::s_WaitingLocked=nullptr;

}