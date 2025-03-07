//============
// Signal.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Scheduler.h"
#include "Signal.h"
#include "SpinLock.h"
#include "Task.h"

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

VOID Signal::Trigger(BOOL cancel)
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_WaitingTask)
	return;
auto waiting=m_WaitingTask;
while(waiting)
	{
	if(waiting->m_ResumeTime)
		Scheduler::s_WaitingTask=Scheduler::RemoveWaitingTask(Scheduler::s_WaitingTask, waiting);
	waiting->m_ResumeTime=(cancel? 1: 0);
	waiting=waiting->m_Parallel;
	}
Scheduler::ResumeTask(m_WaitingTask);
m_WaitingTask=nullptr;
}

BOOL Signal::Wait()
{
SpinLock lock(Scheduler::s_CriticalSection);
assert(!Task::IsMainTask());
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
lock.Yield();
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}

BOOL Signal::Wait(UINT timeout)
{
assert(timeout!=0);
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(timeout);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
lock.Yield();
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}

BOOL Signal::Wait(ScopedLock& scoped_lock)
{
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
scoped_lock.Yield(lock);
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}

BOOL Signal::Wait(ScopedLock& scoped_lock, UINT timeout)
{
assert(timeout!=0);
assert(!Task::IsMainTask());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(timeout);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
scoped_lock.Yield(lock);
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}


//================
// Common Private
//================

BOOL Signal::WaitInternal()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
lock.Yield();
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}

}