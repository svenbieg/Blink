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
#include "Task.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID Signal::Broadcast()
{
SpinLock task_lock(Scheduler::s_CriticalSection);
if(!m_WaitingTask)
	return;
Scheduler::ResumeTask(m_WaitingTask);
m_WaitingTask=nullptr;
}


//================
// Common Private
//================

BOOL Signal::Wait(ScopedLock* scoped_lock, UINT timeout)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
if(!timeout)
	{
	Scheduler::SuspendCurrentTask(nullptr, 0);
	}
else
	{
	Scheduler::s_WaitingTask=Scheduler::SuspendCurrentTask(Scheduler::s_WaitingTask, timeout);
	}
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
if(scoped_lock)
	scoped_lock->Unlock();
lock.Yield(); // Waiting...
if(scoped_lock)
	scoped_lock->Lock();
BOOL signal=(task->m_ResumeTime==0);
if(!signal)
	{
	m_WaitingTask=Scheduler::RemoveParallelTask(m_WaitingTask, task);
	task->m_ResumeTime=0;
	}
return signal;
}

}
