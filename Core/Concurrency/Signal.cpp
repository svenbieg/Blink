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


//==================
// Con-/Destructors
//==================

Signal::Signal()
{}


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

BOOL Signal::Wait(ScopedLock& lock, UINT time_out)
{
SpinLock task_lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
if(!task)
	return true;
if(!time_out)
	{
	Scheduler::SuspendCurrentTask(nullptr, 0);
	}
else
	{
	Scheduler::s_WaitingTask=Scheduler::SuspendCurrentTask(Scheduler::s_WaitingTask, time_out);
	}
m_WaitingTask=Scheduler::AddParallelTask(m_WaitingTask, task);
lock.Unlock();
task_lock.Unlock();
// Waiting...
lock.Lock();
return task->m_ResumeTime==0;
}

}
