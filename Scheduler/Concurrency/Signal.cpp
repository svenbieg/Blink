//============
// Signal.cpp
//============

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#signal

#include "Concurrency/Task.h"
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
if(m_Waiting)
	{
	Scheduler::ResumeTask(m_Waiting, status);
	m_Waiting=nullptr;
	}
}

VOID Signal::Wait()
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current);
lock.Yield();
current->m_Signal=nullptr;
lock.Unlock();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Wait(UINT timeout)
{
assert(!Task::IsMainTask());
UINT64 resume_time=SystemTimer::GetTickCount64()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current, resume_time);
lock.Yield();
current->m_Signal=nullptr;
lock.Unlock();
if(current->m_ResumeTime)
	throw TimeoutException();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Wait(ScopedLock& scoped_lock)
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
lock.Unlock();
StatusHelper::ThrowIfFailed(current->m_Status);
}

VOID Signal::Wait(ScopedLock& scoped_lock, UINT timeout)
{
assert(timeout!=0);
assert(!Task::IsMainTask());
UINT64 resume_time=SystemTimer::GetTickCount64()+timeout;
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current, resume_time);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
lock.Unlock();
if(current->m_ResumeTime)
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
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(core, current);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
}

}