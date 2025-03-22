//============
// Signal.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Concurrency/Signal.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"
#include "StatusHelper.h"

using namespace Devices::System;


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
if(!m_Waiting)
	return;
auto waiting=m_Waiting;
m_Waiting=nullptr;
if(waiting->m_ResumeTime)
	{
	Scheduler::RemoveSleepingTask(&Scheduler::s_Sleeping, waiting);
	waiting->m_ResumeTime=0;
	}
Scheduler::ResumeTask(waiting, status);
}

VOID Signal::Wait()
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(nullptr, core, current);
lock.Yield();
current->m_Signal=nullptr;
if(current->Cancelled)
	throw AbortException();
if(current->m_ResumeTime)
	throw TimeoutException();
}

VOID Signal::Wait(UINT timeout)
{
assert(timeout!=0);
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(timeout);
lock.Yield();
current->m_Signal=nullptr;
if(current->Cancelled)
	throw AbortException();
if(current->m_ResumeTime)
	throw TimeoutException();
}

VOID Signal::Wait(ScopedLock& scoped_lock)
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(nullptr, core, current);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
if(current->Cancelled)
	throw AbortException();
if(current->m_ResumeTime)
	throw TimeoutException();
}

VOID Signal::Wait(ScopedLock& scoped_lock, UINT timeout)
{
assert(timeout!=0);
SpinLock lock(Scheduler::s_CriticalSection);
assert(!Task::IsMainTask());
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::AddParallelTask(&m_Waiting, current);
Scheduler::SuspendCurrentTask(timeout);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
if(current->Cancelled)
	throw AbortException();
if(current->m_ResumeTime)
	throw TimeoutException();
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
Scheduler::SuspendCurrentTask(nullptr, core, current);
scoped_lock.Yield(lock);
current->m_Signal=nullptr;
}

}