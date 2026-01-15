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
UINT core=Cpu::GetId();
auto idle=Task::CreateInternal(IdleTask, String::Create("idle%u", core));
FlagHelper::Set(idle->m_Flags, TaskFlags::Idle);
if(core==0)
	{
	Interrupts::Route(Irq::TaskSwitch, IrqTarget::All);
	Interrupts::SetHandler(Irq::TaskSwitch, HandleTaskSwitch);
	auto main=Task::CreateInternal(MainTask, "main");
	s_MainTask=main;
	}
SpinLock lock(s_CriticalSection);
s_IdleTask[core]=idle;
auto task=idle;
if(core==0)
	task=s_MainTask;
s_CurrentTask[core]=task;
lock.Unlock();
Cpu::SetContext(&Task::TaskProc, task, task->m_StackPointer);
}


//================
// Common Private
//================

VOID Scheduler::AddTask(Task* task)
{
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Creator);
task->m_Creator=current;
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

UINT Scheduler::CreateTasks()
{
UINT count=0;
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
	count++;
	}
return count;
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

UINT Scheduler::GetNextCore(BOOL suspend)
{
for(UINT u=0; u<CPU_COUNT; u++)
	{
	UINT core=s_CurrentCore;
	s_CurrentCore=(core+1)%CPU_COUNT;
	auto task=s_CurrentTask[core];
	if(!task)
		continue;
	if(FlagHelper::Get(task->m_Flags, TaskFlags::Idle))
		return core;
	if(!suspend)
		continue;
	if(!FlagHelper::Get(task->m_Flags, TaskFlags::Priority))
		return core;
	}
return CPU_COUNT;
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

VOID Scheduler::ResumeWaitingTasks(UINT count, BOOL suspend)
{
for(UINT u=0; u<count; u++)
	{
	UINT core=GetNextCore(suspend);
	if(core==CPU_COUNT)
		break;
	auto current=s_CurrentTask[core];
	current->m_Next=s_Waiting.RemoveFirst();
	Interrupts::Send(Irq::TaskSwitch, core);
	}
}

VOID Scheduler::Schedule()
{
SpinLock lock(s_CriticalSection);
auto release=s_Release.RemoveFirst();
while(release)
	{
	lock.Unlock();
	release->m_This=nullptr;
	lock.Lock();
	release=s_Release.RemoveFirst();
	}
auto sleeping=s_Sleeping.First();
if(sleeping)
	{
	UINT64 time=SystemTimer::GetTickCount64();
	while(sleeping)
		{
		if(sleeping->m_ResumeTime>time)
			break;
		s_Sleeping.RemoveFirst();
		FlagHelper::Clear(sleeping->m_Flags, TaskFlags::Suspended);
		FlagHelper::Set(sleeping->m_Flags, TaskFlags::Timeout);
		sleeping->m_ResumeTime=0;
		s_Waiting.Append(sleeping);
		sleeping=s_Sleeping.First();
		}
	}
UINT waiting_count=s_Waiting.Count(CPU_COUNT);
if(waiting_count)
	ResumeWaitingTasks(waiting_count, true);
}

VOID Scheduler::SuspendCurrentTask(UINT ms)
{
UINT64 resume_time=SystemTimer::GetTickCount64()+ms;
SpinLock lock(s_CriticalSection);
UINT core=Cpu::GetId();
auto current=s_CurrentTask[core];
SuspendCurrentTask(core, current, resume_time);
lock.Unlock();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Scheduler::SuspendCurrentTask(UINT core, Task* current, UINT64 resume_time)
{
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
UINT resume_count=0;
if(FlagHelper::Get(current->m_Flags, TaskFlags::Creator))
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Creator);
	resume_count=CreateTasks();
	}
if(resume_time)
	{
	current->m_ResumeTime=resume_time;
	s_Sleeping.Insert(current, [](Task* first, Task* second){ return first->m_ResumeTime<second->m_ResumeTime; });
	}
if(resume_count)
	ResumeWaitingTasks(resume_count, false);
if(!current->m_Next)
	{
	auto resume=s_Waiting.RemoveFirst();
	if(!resume)
		resume=s_IdleTask[core];
	current->m_Next=resume;
	Interrupts::Send(Irq::TaskSwitch, core);
	}
}

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