//===========
// Mutex.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

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
auto current=Scheduler::s_CurrentTask[core];
if(m_Owner)
	{
	Scheduler::SuspendCurrentTask(m_Owner);
	lock.Yield();
	}
m_Owner=current;
}

VOID Mutex::LockBlocking()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current)
	return;
current->SetFlag(TaskFlags::Blocking);
current->m_BlockingCount++;
if(m_Owner)
	{
	Scheduler::SuspendCurrentTask(m_Owner);
	lock.Yield();
	}
m_Owner=current;
}

VOID Mutex::LockShared()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->SetFlag(TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
auto waiting=m_Owner;
while(waiting->m_Waiting)
	waiting=waiting->m_Waiting;
if(waiting->GetFlag(TaskFlags::Sharing))
	{
	Scheduler::AddParallelTask(waiting, current);
	if(waiting==m_Owner)
		return;
	Scheduler::SuspendCurrentTask(nullptr);
	}
else
	{
	Scheduler::SuspendCurrentTask(waiting);
	}
lock.Yield();
m_Owner=Scheduler::AddParallelTask(m_Owner, current);
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

BOOL Mutex::TryLockBlocking()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
m_Owner=Scheduler::s_CurrentTask[core];
m_Owner->SetFlag(TaskFlags::Blocking);
m_Owner->m_BlockingCount++;
return true;
}

BOOL Mutex::TryLockShared()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner&&!m_Owner->GetFlag(TaskFlags::Sharing))
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->SetFlag(TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
Scheduler::AddParallelTask(m_Owner, current);
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
assert(m_Owner==Scheduler::s_CurrentTask[core]);
if(!m_Owner->m_Waiting)
	{
	m_Owner=nullptr;
	return;
	}
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
Scheduler::ResumeTask(waiting);
}

VOID Mutex::UnlockBlocking()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner)
	return;
UINT core=Cpu::GetId();
assert(m_Owner==Scheduler::s_CurrentTask[core]);
if(--m_Owner->m_BlockingCount==0)
	m_Owner->ClearFlag(TaskFlags::Blocking);
if(!m_Owner->m_Waiting)
	{
	m_Owner=nullptr;
	return;
	}
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
Scheduler::ResumeTask(waiting);
}

VOID Mutex::UnlockShared()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
current->ClearFlag(TaskFlags::Sharing);
auto parallel=Scheduler::RemoveParallelTask(m_Owner, current);
if(parallel)
	{
	m_Owner=parallel;
	return;
	}
if(!m_Owner->m_Waiting)
	{
	m_Owner=nullptr;
	return;
	}
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
Scheduler::ResumeTask(waiting);
}

}
