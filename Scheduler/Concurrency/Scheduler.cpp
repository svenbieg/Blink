//===============
// Scheduler.cpp
//===============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler

#include "Scheduler.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"
#include "FlagHelper.h"
#include "StatusHelper.h"

using namespace Devices::System;
using namespace Devices::Timers;

extern VOID Main();


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
++s_CoreCount;
UINT core=Cpu::GetId();
auto task=s_CurrentTask[core];
lock.Unlock();
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}

VOID Scheduler::Initialize()
{
Interrupts::Route(Irq::TaskSwitch, IrqTarget::All);
Interrupts::SetHandler(Irq::TaskSwitch, HandleTaskSwitch);
for(UINT core=0; core<CPU_COUNT; core++)
	{
	auto idle=Task::CreateInternal(IdleTask, String::Create("idle%u", core));
	FlagHelper::Set(idle->m_Flags, TaskFlags::Idle);
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


//==================
// Common Protected
//==================

VOID Scheduler::AddTask(Task* task)
{
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
	WakeupTasks(task, Status::Aborted);
}

VOID Scheduler::ExitTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
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
		FlagHelper::Set(current->m_Flags, TaskFlags::Timeout);
		current->m_ResumeTime=0;
		AddWaitingTask(current);
		*current_ptr=next;
		}
	}
ResumeWaitingTasks();
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
UINT64 resume_time=SystemTimer::GetTickCount64()+ms;
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
SuspendCurrentTask(core, current, resume_time);
lock.Unlock();
StatusHelper::ThrowIfFailed(current->m_Status);
}


//================
// Common Private
//================

VOID Scheduler::AddCreateTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	current_ptr=&current->m_Create;
	}
*current_ptr=task;
}

VOID Scheduler::AddParallelTask(Task** current_ptr, Task* task)
{
BOOL locked=FlagHelper::Get(task->m_Flags, TaskFlags::Locked);
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	if(locked&&!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		{
		task->m_Parallel=current;
		*current_ptr=task;
		return;
		}
	current_ptr=&current->m_Parallel;
	}
*current_ptr=task;
}

VOID Scheduler::AddReleaseTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	current_ptr=&current->m_Release;
	}
*current_ptr=task;
}

VOID Scheduler::AddSleepingTask(Task** current_ptr, Task* task)
{
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	if(current->m_ResumeTime>task->m_ResumeTime)
		{
		task->m_Sleeping=current;
		*current_ptr=task;
		return;
		}
	current_ptr=&current->m_Sleeping;
	}
*current_ptr=task;
}

VOID Scheduler::AddWaitingTask(Task* task)
{
assert(!FlagHelper::Get(task->m_Flags, TaskFlags::Suspended));
if(FlagHelper::Get(task->m_Flags, TaskFlags::Idle))
	return;
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
	s_WaitingLocked=task;
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
	auto next=current->m_Create;
	if(!FlagHelper::Get(owner->m_Flags, TaskFlags::Owner))
		{
		current->m_Owner=nullptr;
		current->m_Create=nullptr;
		AddWaitingTask(current);
		}
	*current_ptr=next;
	}
}

UINT Scheduler::GetAvailableCores(UINT* cores, UINT max)
{
UINT count=0;
UINT mask=0;
static_assert(CPU_COUNT<=32, "CPU_COUNT>32");
for(UINT core=0; core<s_CoreCount; core++)
	{
	auto current=s_CurrentTask[core];
	auto next=current->m_Next;
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		{
		if(!next)
			continue;
		}
	if(next)
		{
		if(FlagHelper::Get(next->m_Flags, TaskFlags::Locked))
			continue;
		}
	cores[count++]=core;
	if(count==max)
		return count;
	mask|=(1UL<<core);
	}
return count;
}

UINT Scheduler::GetWaitingCount(Task* waiting, UINT max)
{
UINT count=0;
while(waiting)
	{
	if(++count==max)
		break;
	waiting=waiting->m_Waiting;
	}
return count;
}

Task* Scheduler::GetWaitingTask()
{
auto resume=s_WaitingFirst;
auto waiting=resume->m_Waiting;
resume->m_Waiting=nullptr;
s_WaitingFirst=waiting;
if(waiting)
	{
	if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Locked))
		{
		s_WaitingLocked=waiting;
		}
	else
		{
		s_WaitingLocked=nullptr;
		}
	}
else
	{
	s_WaitingLast=nullptr;
	s_WaitingLocked=nullptr;
	}
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
		{
		AddReleaseTask(&s_Release, current);
		}
	}
else
	{
	AddWaitingTask(current);
	}
s_CurrentTask[core]=next;
TaskHelper::Switch(core, current, next);
lock.Unlock();
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
		current->m_Parallel=nullptr;
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

VOID Scheduler::ResumeWaitingTask(UINT core, Task* current)
{
auto next=current->m_Next;
if(next)
	{
	if(!s_WaitingLocked)
		return;
	if(FlagHelper::Get(next->m_Flags, TaskFlags::Locked))
		return;
	current->m_Next=GetWaitingTask();
	AddWaitingTask(next);
	}
else
	{
	if(!s_WaitingFirst)
		return;
	current->m_Next=GetWaitingTask();
	Interrupts::Send(Irq::TaskSwitch, core);
	}
}

VOID Scheduler::ResumeWaitingTasks()
{
UINT waiting_count=GetWaitingCount(s_WaitingFirst, s_CoreCount);
if(!waiting_count)
	return;
UINT cores[CPU_COUNT];
UINT core_count=GetAvailableCores(cores, waiting_count);
for(UINT core_id=0; core_id<core_count; core_id++)
	{
	UINT core=cores[core_id];
	auto current=s_CurrentTask[core];
	ResumeWaitingTask(core, current);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT core, Task* current, UINT64 resume_time)
{
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
auto next=current->m_Next;
if(next)
	{
	if(!s_WaitingLocked)
		return;
	if(FlagHelper::Get(next->m_Flags, TaskFlags::Locked))
		return;
	current->m_Next=GetWaitingTask();
	AddWaitingTask(next);
	return;
	}
auto resume=GetWaitingTask();
if(!resume)
	resume=s_IdleTask[core];
current->m_Next=resume;
Interrupts::Send(Irq::TaskSwitch, core);
}

VOID Scheduler::WakeupTasks(Task* wakeup, Status status)
{
while(wakeup)
	{
	if(wakeup->m_ResumeTime)
		{
		RemoveSleepingTask(&Scheduler::s_Sleeping, wakeup);
		wakeup->m_ResumeTime=0;
		}
	auto parallel=wakeup->m_Parallel;
	wakeup->m_Parallel=nullptr;
	wakeup->m_Signal=nullptr;
	wakeup->m_Status=status;
	FlagHelper::Clear(wakeup->m_Flags, TaskFlags::Suspended);
	AddWaitingTask(wakeup);
	wakeup=parallel;
	}
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