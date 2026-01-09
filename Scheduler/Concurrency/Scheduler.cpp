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
#include "Concurrency/TaskHelper.h"
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
s_All.Append(task);
s_Create.Append(task);
}

VOID Scheduler::CancelTask(Task* task)
{
SpinLock lock(s_CriticalSection);
BOOL resume=false;
if(task->m_ResumeTime)
	{
	s_Sleeping.Remove(task);
	task->m_ResumeTime=0;
	resume=true;
	}
auto signal=task->m_Signal;
if(signal)
	{
	WaitingList::Remove(&signal->m_Waiting, task);
	task->m_Signal=nullptr;
	resume=true;
	}
if(resume)
	{
	FlagHelper::Clear(task->m_Flags, TaskFlags::Suspended);
	task->m_Status=Status::Aborted;
	s_Waiting.Append(task);
	}
}

VOID Scheduler::ExitTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
s_All.Remove(current);
FlagHelper::Set(current->m_Flags, TaskFlags::ReleaseSuspended);
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

BOOL Scheduler::IsMainTask()
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
return current==s_MainTask;
}

VOID Scheduler::Schedule()
{
SpinLock lock(s_CriticalSection);
if(s_Release)
	{
	auto release=s_Release.RemoveFirst();
	while(release)
		{
		lock.Unlock();
		release->m_This=nullptr;
		lock.Lock();
		release=s_Release.RemoveFirst();
		}
	}
if(s_Sleeping)
	{
	UINT64 time=SystemTimer::GetTickCount64();
	auto sleeping=s_Sleeping.First();
	while(sleeping)
		{
		if(sleeping->m_ResumeTime>time)
			break;
		s_Sleeping.RemoveFirst();
		FlagHelper::Clear(sleeping->m_Flags, TaskFlags::Suspended);
		FlagHelper::Set(sleeping->m_Flags, TaskFlags::Timeout);
		sleeping->m_ResumeTime=0;
		s_Waiting.Insert(sleeping, Task::Priority);
		sleeping=s_Sleeping.First();
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

VOID Scheduler::CreateTasks()
{
auto create=s_Create.First();
while(create)
	{
	auto owner=create->m_Owner;
	if(FlagHelper::Get(owner->m_Flags, TaskFlags::Owner))
		{
		create=s_Create.Next(create);
		}
	else
		{
		auto next=s_Create.Remove(create);
		create->m_Owner=nullptr;
		s_Waiting.Insert(create, Task::Priority);
		create=next;
		}
	}
}

UINT Scheduler::GetAvailableCores(UINT* cores, UINT max)
{
UINT count=0;
static_assert(CPU_COUNT<=32);
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
	}
return count;
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
		s_Release.Append(current);
	}
else
	{
	if(!FlagHelper::Get(current->m_Flags, TaskFlags::Idle))
		s_Waiting.Append(current);
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

VOID Scheduler::ResumeWaitingTask(UINT core, Task* current, BOOL suspend)
{
auto next=current->m_Next;
if(next)
	{
	if(FlagHelper::Get(next->m_Flags, TaskFlags::Locked))
		return;
	auto waiting=s_Waiting.First();
	if(!waiting)
		return;
	if(!FlagHelper::Get(waiting->m_Flags, TaskFlags::Locked))
		return;
	s_Waiting.RemoveFirst();
	current->m_Next=waiting;
	if(!FlagHelper::Get(next->m_Flags, TaskFlags::Idle))
		s_Waiting.Insert(next, &Task::Priority);
	return;
	}
auto resume=s_Waiting.RemoveFirst();
if(!resume)
	{
	if(!suspend)
		return;
	resume=s_IdleTask[core];
	}
current->m_Next=resume;
Interrupts::Send(Irq::TaskSwitch, core);
}

VOID Scheduler::ResumeWaitingTasks()
{
UINT waiting_count=s_Waiting.Count(s_CoreCount);
if(!waiting_count)
	return;
UINT cores[CPU_COUNT];
UINT core_count=GetAvailableCores(cores, waiting_count);
for(UINT core_id=0; core_id<core_count; core_id++)
	{
	UINT core=cores[core_id];
	auto current=s_CurrentTask[core];
	ResumeWaitingTask(core, current, false);
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
	s_Sleeping.Insert(current, [](Task* first, Task* second){ return first->m_ResumeTime<second->m_ResumeTime; });
	}
ResumeWaitingTask(core, current, true);
}

Scheduler::AllList Scheduler::s_All;
UINT Scheduler::s_CoreCount=0;
Scheduler::CreateList Scheduler::s_Create;
CriticalSection Scheduler::s_CriticalSection;
Task* Scheduler::s_CurrentTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_IdleTask[CPU_COUNT]={ nullptr };
Task* Scheduler::s_MainTask=nullptr;
Scheduler::ReleaseList Scheduler::s_Release;
Scheduler::SleepingList Scheduler::s_Sleeping;
Scheduler::WaitingList Scheduler::s_Waiting;

}