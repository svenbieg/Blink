//===============
// Scheduler.cpp
//===============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler

#include "Scheduler.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"
#include "FlagHelper.h"
#include "StatusHelper.h"

using namespace Collections;
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
s_CriticalSection.Lock();
UINT core=Cpu::GetId();
auto task=s_IdleTask[core];
s_CurrentTask[core]=task;
FlagHelper::Set(task->m_Flags, TaskFlags::Active);
task->m_StackPointer=task->m_StackTop;
TaskMonitor::SetTask(core, task);
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}

VOID Scheduler::Initialize()
{
s_MainTask=Task::CreateInternal(MainTask, "main");
s_All.Append(s_MainTask);
for(UINT core=0; core<CPU_COUNT; core++)
	{
	auto idle=Task::CreateInternal(IdleTask, String::Create("idle%u", core), 1024);
	FlagHelper::Set(idle->m_Flags, TaskFlags::Idle);
	s_All.Append(idle);
	s_IdleTask[core]=idle;
	}
}


//==================
// Common Protected
//==================

VOID Scheduler::AddTask(Task* task)noexcept
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Creator);
task->m_Creator=current;
s_All.Append(task);
s_Create.Append(task);
}

VOID Scheduler::CancelTask(Task* task)noexcept
{
SpinLock lock(s_CriticalSection);
task->Cancelled=true;
task->m_Status=Status::Aborted;
BOOL resume=false;
if(task->m_ResumeTime)
	{
	s_Sleeping.Remove(task);
	task->m_ResumeTime=0;
	resume=true;
	}
auto owner=task->m_Owner;
if(owner)
	{
	WaitingList::Remove(owner, task);
	if(FlagHelper::Get(task->m_OwnerFlags, TaskFlags::Priority))
		{
		if(--task->m_PriorityCount==0)
			FlagHelper::Clear(task->m_Flags, TaskFlags::Priority);
		}
	FlagHelper::Clear(task->m_Flags, TaskFlags::Sharing);
	task->m_Owner=nullptr;
	task->m_OwnerFlags=TaskFlags::None;
	task->m_SignalCount=0;
	task->m_SignalLock=nullptr;
	resume=true;
	}
if(resume)
	Resume(task);
}

VOID Scheduler::ExitTask()noexcept
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Release);
Suspend(core, current);
lock.Unlock();
while(1)
	Cpu::WaitForInterrupt();
}

Task* Scheduler::GetCurrentTask()noexcept
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
return s_CurrentTask[core];
}

VOID Scheduler::HandleTaskSwitch()noexcept
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
assert(current->z_StackHeader==Task::STACK_HEADER); // Stack-overflow
auto next=current->m_Next;
if(!next)
	return;
current->m_Next=nullptr;
if(FlagHelper::Get(current->m_Flags, TaskFlags::Suspend))
	{
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Release))
		s_Release.Append(current);
	}
else
	{
	if(!FlagHelper::Get(current->m_Flags, TaskFlags::Idle))
		s_Waiting.Append(current);
	}
s_CurrentTask[core]=next;
FlagHelper::Clear(current->m_Flags, TaskFlags::Active);
FlagHelper::Set(next->m_Flags, TaskFlags::Active);
TaskHelper::Switch(core, current, next);
TaskMonitor::SetTask(core, next);
}

VOID Scheduler::Schedule()noexcept
{
SpinLock lock(s_CriticalSection);
auto release=s_Release.RemoveFirst();
while(release)
	{
	TaskMonitor::RemoveTask(release);
	s_All.Remove(release);
	lock.Unlock();
	release->m_This=nullptr;
	lock.Lock();
	release=s_Release.RemoveFirst();
	}
auto sleeping=s_Sleeping.First();
if(sleeping)
	{
	UINT64 time=SystemTimer::GetTickCount();
	while(sleeping)
		{
		if(sleeping->m_ResumeTime>time)
			break;
		auto next=s_Sleeping.Remove(sleeping);
		FlagHelper::Clear(sleeping->m_Flags, TaskFlags::Suspend);
		FlagHelper::Set(sleeping->m_Flags, TaskFlags::Timeout);
		sleeping->m_ResumeTime=0;
		auto owner=sleeping->m_Owner;
		if(owner)
			{
			WaitingList::Remove(owner, sleeping);
			sleeping->m_Owner=nullptr;
			sleeping->m_SignalCount=0;
			sleeping->m_SignalLock=nullptr;
			}
		s_Waiting.Insert(sleeping, Task::Priority);
		sleeping=next;
		}
	}
for(UINT u=0; u<CPU_COUNT; u++)
	{
	auto resume=s_Waiting.First();
	if(!resume)
		break;
	UINT core=s_CurrentCore;
	s_CurrentCore=(core+1)%CPU_COUNT;
	auto current=s_CurrentTask[core];
	if(!current)
		continue;
	auto next=current->m_Next;
	if(next)
		{
		if(!FlagHelper::Get(next->m_Flags, TaskFlags::Idle))
			continue;
		s_Waiting.RemoveFirst();
		current->m_Next=resume;
		continue;
		}
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Priority))
		continue;
	s_Waiting.RemoveFirst();
	current->m_Next=resume;
	Interrupts::Send(Irq::TaskSwitch, core);
	}
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
UINT64 resume_time=SystemTimer::GetTickCount()+ms;
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
Suspend(core, current, resume_time);
lock.Unlock();
StatusHelper::ThrowIfFailed(current->m_Status);
}


//================
// Common Private
//================

VOID Scheduler::CreateTasks()noexcept
{
auto create=s_Create.First();
while(create)
	{
	auto creator=create->m_Creator;
	if(FlagHelper::Get(creator->m_Flags, TaskFlags::Creator))
		{
		create=s_Create.Next(create);
		continue;
		}
	auto next=s_Create.Remove(create);
	create->m_Creator=nullptr;
	s_Waiting.Insert(create, Task::Priority);
	create=next;
	}
}

VOID Scheduler::IdleTask()
{
UINT core=Cpu::GetId();
if(core==0)
	Resume(s_MainTask);
s_CriticalSection.Unlock();
Interrupts::Enable();
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

VOID Scheduler::MainTask()
{
auto status=Status::Success;
try
	{
	SystemTimer::Start();
	Main();
	}
catch(...)
	{
	status=Status::Error;
	}
System::Restart();
}

VOID Scheduler::Resume(Task* resume)noexcept
{
FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspend);
if(FlagHelper::Get(resume->m_Flags, TaskFlags::Active))
	{
	auto next=resume->m_Next;
	if(next)
		{
		if(FlagHelper::Get(next->m_Flags, TaskFlags::Idle))
			resume->m_Next=nullptr;
		}
	return;
	}
for(UINT u=0; u<CPU_COUNT; u++)
	{
	UINT core=s_CurrentCore;
	s_CurrentCore=(core+1)%CPU_COUNT;
	auto current=s_CurrentTask[core];
	if(!current)
		continue;
	auto next=current->m_Next;
	if(next)
		{
		if(!FlagHelper::Get(next->m_Flags, TaskFlags::Idle))
			continue;
		current->m_Next=resume;
		Interrupts::Send(Irq::TaskSwitch, core);
		return;
		}
	if(FlagHelper::Get(current->m_Flags, TaskFlags::Priority))
		continue;
	BOOL idle=FlagHelper::Get(current->m_Flags, TaskFlags::Idle);
	BOOL priority=FlagHelper::Get(resume->m_Flags, TaskFlags::Priority);
	if(idle||priority)
		{
		current->m_Next=resume;
		Interrupts::Send(Irq::TaskSwitch, core);
		return;
		}
	}
s_Waiting.Insert(resume, Task::Priority);
}

VOID Scheduler::Suspend(UINT core, Task* task, UINT64 resume_time)noexcept
{
FlagHelper::Set(task->m_Flags, TaskFlags::Suspend);
if(FlagHelper::Get(task->m_Flags, TaskFlags::Creator))
	{
	FlagHelper::Clear(task->m_Flags, TaskFlags::Creator);
	CreateTasks();
	}
if(resume_time)
	{
	task->m_ResumeTime=resume_time;
	s_Sleeping.Insert(task, [](Task* first, Task* second){ return first->m_ResumeTime<second->m_ResumeTime; });
	}
if(task->m_Next)
	return;
auto resume=s_Waiting.RemoveFirst();
if(!resume)
	resume=s_IdleTask[core];
task->m_Next=resume;
Interrupts::Send(Irq::TaskSwitch, core);
}

Scheduler::AllList Scheduler::s_All;
Scheduler::CreateList Scheduler::s_Create;
CriticalSection Scheduler::s_CriticalSection;
UINT Scheduler::s_CurrentCore=0;
Task* Scheduler::s_CurrentTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_IdleTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_MainTask=nullptr;
Scheduler::ReleaseList Scheduler::s_Release;
Scheduler::SleepingList Scheduler::s_Sleeping;
Scheduler::WaitingList Scheduler::s_Waiting;

}