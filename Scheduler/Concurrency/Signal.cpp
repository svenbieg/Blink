//============
// Signal.cpp
//============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#signal

#include "Signal.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/SpinLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/Timers/SystemTimer.h"
#include "StatusHelper.h"

using namespace Devices::System;
using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID Signal::Trigger(Status status)
{
SpinLock lock(Scheduler::s_CriticalSection);
auto wakeup=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
while(wakeup)
	{
	if(wakeup->m_ResumeTime)
		{
		Scheduler::s_Sleeping.Remove(wakeup);
		wakeup->m_ResumeTime=0;
		}
	FlagHelper::Clear(wakeup->m_Flags, TaskFlags::Suspended);
	wakeup->m_Status=status;
	Scheduler::s_Waiting.Insert(wakeup, Task::Priority);
	wakeup=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
Scheduler::ResumeWaitingTask(core, current, false);
}

VOID Signal::Wait(UINT timeout)
{
assert(!Task::IsMainTask());
UINT64 resume_time=0;
if(timeout)
	resume_time=SystemTimer::GetTickCount64()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_Signal=this;
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
Scheduler::SuspendCurrentTask(core, current, resume_time);
lock.Unlock();
if(FlagHelper::Get(current->m_Flags, TaskFlags::Timeout))
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Wait(ScopedLock& scoped_lock, UINT timeout)
{
assert(!Task::IsMainTask());
UINT64 resume_time=0;
if(timeout)
	resume_time=SystemTimer::GetTickCount64()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_Signal=this;
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
Scheduler::SuspendCurrentTask(core, current, resume_time);
scoped_lock.Yield(lock);
lock.Unlock();
if(FlagHelper::Get(current->m_Flags, TaskFlags::Timeout))
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
}


//================
// Common Private
//================

VOID Signal::WaitInternal(ScopedLock& scoped_lock)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
current->m_Signal=this;
Scheduler::WaitingList::Append(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current);
scoped_lock.Yield(lock);
}

}