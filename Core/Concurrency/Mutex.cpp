//===========
// Mutex.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Mutex.h"
#include "Scheduler.h"
#include "Task.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID Mutex::Lock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
if(!m_Owner)
	{
	m_Owner=task;
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner, 0);
lock.Yield();
m_Owner->m_Waiting=nullptr;
m_Owner=task;
}

VOID Mutex::Lock(BOOL blocking)
{
if(!blocking)
	return Lock();
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
if(!task)
	return;
task->SetFlag(TaskFlags::Blocking);
if(!m_Owner)
	{
	m_Owner=task;
	Interrupts::Disable();
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner, 0);
lock.Yield();
m_Owner->m_Waiting=nullptr;
m_Owner=task;
}

VOID Mutex::LockShared()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto task=Scheduler::s_CurrentTask[core];
if(!task)
	return;
task->SetFlag(TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=task;
	return;
	}
auto waiting=m_Owner;
while(waiting->m_Waiting)
	waiting=waiting->m_Waiting;
if(waiting->GetFlag(TaskFlags::Sharing))
	{
	Scheduler::AddParallelTask(waiting, task);
	if(waiting==m_Owner)
		return;
	Scheduler::SuspendCurrentTask(nullptr, 0);
	}
else
	{
	Scheduler::SuspendCurrentTask(waiting, 0);
	}
lock.Yield();
m_Owner->m_Waiting=nullptr;
m_Owner=task;
}

BOOL Mutex::TryLock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
m_Owner=Scheduler::s_CurrentTask[core];
return true;
}

BOOL Mutex::TryLock(BOOL blocking)
{
if(!blocking)
	return TryLock();
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
m_Owner=Scheduler::s_CurrentTask[core];
if(m_Owner)
	{
	m_Owner->SetFlag(TaskFlags::Blocking);
	Interrupts::Disable();
	}
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner)
	return;
if(m_Owner->GetFlag(TaskFlags::Blocking))
	{
	Interrupts::Enable();
	m_Owner->ClearFlag(TaskFlags::Blocking);
	}
auto waiting=m_Owner->m_Waiting;
if(!waiting)
	{
	m_Owner=nullptr;
	return;
	}
Cpu::DataStoreBarrier();
Scheduler::ResumeTask(waiting);
}

VOID Mutex::UnlockShared()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current)
	return;
current->ClearFlag(TaskFlags::Sharing);
auto parallel=Scheduler::RemoveParallelTask(m_Owner, current);
if(parallel)
	{
	m_Owner=parallel;
	return;
	}
auto waiting=m_Owner->m_Waiting;
if(!waiting)
	{
	m_Owner=nullptr;
	return;
	}
Scheduler::ResumeTask(waiting);
}

}
