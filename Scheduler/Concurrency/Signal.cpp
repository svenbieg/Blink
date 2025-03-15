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


//==================
// Con-/Destructors
//==================

Signal::Signal()
{}

Signal::~Signal()
{}


//========
// Common
//========

VOID Signal::Trigger(Status status)
{
SpinLock lock(Scheduler::s_CriticalSection);
auto waiting=m_WaitingTask;
m_WaitingTask=nullptr;
Scheduler::ResumeTask(waiting, status);
}

VOID Signal::Wait()
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::SuspendCurrentTask(nullptr);
Scheduler::AddParallelTask(&m_WaitingTask, current);
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
Scheduler::SuspendCurrentTask(timeout);
Scheduler::AddParallelTask(&m_WaitingTask, current);
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
Scheduler::SuspendCurrentTask(nullptr);
Scheduler::AddParallelTask(&m_WaitingTask, current);
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
Scheduler::SuspendCurrentTask(timeout);
Scheduler::AddParallelTask(&m_WaitingTask, current);
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

VOID Signal::RemoveWaitingTask(Task* task)
{
SpinLock lock(Scheduler::s_CriticalSection);
Scheduler::RemoveParallelTask(&m_WaitingTask, task);
}

BOOL Signal::WaitInternal()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->m_Signal=this;
Scheduler::SuspendCurrentTask(nullptr);
Scheduler::AddParallelTask(&m_WaitingTask, current);
lock.Yield();
current->m_Signal=nullptr;
auto status=current->m_Status;
current->m_Status=Status::Success;
return StatusHelper::Succeeded(status);
}

}