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
if(!m_WaitingTask)
	return;
auto waiting=m_WaitingTask;
while(waiting)
	{
	if(waiting->m_ResumeTime)
		{
		Scheduler::s_WaitingTask=Scheduler::RemoveWaitingTask(Scheduler::s_WaitingTask, waiting);
		waiting->m_ResumeTime=0;
		}
	waiting->m_Status=status;
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
auto current=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, current);
lock.Yield();
return Succeeded(current);
}

BOOL Signal::Wait(UINT timeout)
{
assert(timeout!=0);
SpinLock lock(Scheduler::s_CriticalSection);
assert(!Task::IsMainTask());
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(timeout);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, current);
lock.Yield();
return Succeeded(current);
}

BOOL Signal::Wait(ScopedLock& scoped_lock)
{
SpinLock lock(Scheduler::s_CriticalSection);
assert(!Task::IsMainTask());
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, current);
scoped_lock.Yield(lock);
return Succeeded(current);
}

BOOL Signal::Wait(ScopedLock& scoped_lock, UINT timeout)
{
assert(timeout!=0);
SpinLock lock(Scheduler::s_CriticalSection);
assert(!Task::IsMainTask());
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(timeout);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, current);
scoped_lock.Yield(lock);
return Succeeded(current);
}


//================
// Common Private
//================

BOOL Signal::Succeeded(Task* task)
{
if(task->m_ResumeTime!=0)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	task->m_Status=Status::Timeout;
	}
return StatusHelper::Succeeded(task->m_Status);
}

BOOL Signal::WaitInternal()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
Scheduler::SuspendCurrentTask(nullptr);
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, current);
lock.Yield();
if(current->m_ResumeTime!=0)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, current);
	current->m_ResumeTime=0;
	current->m_Status=Status::Timeout;
	}
return StatusHelper::Succeeded(current->m_Status);
}

}