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

VOID Signal::Count(UINT times, UINT timeout)
{
assert(!Interrupts::Active());
assert(!Task::IsMainTask());
UINT64 resume_time=0;
if(timeout)
	resume_time=SystemTimer::GetTickCount()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_Signal=this;
current->m_SignalCount=times;
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
Scheduler::Suspend(core, current, resume_time);
lock.Unlock();
if(FlagHelper::Get(current->m_Flags, TaskFlags::Timeout))
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Count(ScopedLock& scoped_lock, UINT times, UINT timeout)
{
assert(!Interrupts::Active());
assert(!Task::IsMainTask());
UINT64 resume_time=0;
if(timeout)
	resume_time=SystemTimer::GetTickCount()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
scoped_lock.Unlock(core, current);
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_ScopedLock=&scoped_lock;
current->m_Signal=this;
current->m_SignalCount=times;
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
Scheduler::Suspend(core, current, resume_time);
lock.Unlock();
if(FlagHelper::Get(current->m_Flags, TaskFlags::Timeout))
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Count(SpinLock& spin_lock, UINT times, UINT timeout)
{
assert(!Interrupts::Active());
assert(!Task::IsMainTask());
UINT64 resume_time=0;
if(timeout)
	resume_time=SystemTimer::GetTickCount()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
spin_lock.Unlock();
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_Signal=this;
current->m_SignalCount=times;
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
Scheduler::Suspend(core, current, resume_time);
lock.Unlock();
if(FlagHelper::Get(current->m_Flags, TaskFlags::Timeout))
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
spin_lock.Lock();
}

VOID Signal::Trigger(Status status)noexcept
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto resume=m_Waiting;
while(resume)
	{
	if(--resume->m_SignalCount)
		{
		resume=Scheduler::WaitingList::Next(resume);
		continue;
		}
	if(resume->m_ResumeTime)
		{
		Scheduler::s_Sleeping.Remove(resume);
		resume->m_ResumeTime=0;
		}
	resume->m_Status=status;
	auto next=Scheduler::WaitingList::Remove(&m_Waiting, resume);
	auto scoped_lock=resume->m_ScopedLock;
	if(scoped_lock)
		{
		resume->m_ScopedLock=nullptr;
		if(!scoped_lock->Lock(core, resume))
			continue;
		}
	Scheduler::Resume(resume);
	resume=next;
	}
}


//================
// Common Private
//================

VOID Signal::WaitInternal(SpinLock& spin_lock)
{
SpinLock lock(Scheduler::s_CriticalSection);
spin_lock.Unlock();
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Timeout);
current->m_Signal=this;
current->m_SignalCount=1;
Scheduler::WaitingList::Append(&m_Waiting, current);
Scheduler::Suspend(core, current);
lock.Unlock();
spin_lock.Lock();
}

}