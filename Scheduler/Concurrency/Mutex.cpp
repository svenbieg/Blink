//===========
// Mutex.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Concurrency/Scheduler.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"

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
assert(!current->GetFlag(TaskFlags::Sharing));
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner);
lock.Yield();
}

VOID Mutex::Lock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!current->GetFlag(TaskFlags::Sharing));
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
}

VOID Mutex::Lock(AccessPriority)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(current)
	{
	assert(!current->GetFlag(TaskFlags::Sharing));
	current->Lock();
	}
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner);
lock.Yield();
}

BOOL Mutex::TryLock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!current->GetFlag(TaskFlags::Sharing));
m_Owner=current;
return true;
}

BOOL Mutex::TryLock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner&&!m_Owner->GetFlag(TaskFlags::Sharing))
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!current->GetFlag(TaskFlags::Sharing));
current->SetFlag(TaskFlags::Sharing);
m_Owner=Scheduler::AddParallelTask(m_Owner, current);
return true;
}

BOOL Mutex::TryLock(AccessPriority)
{
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!current->GetFlag(TaskFlags::Sharing));
current->Lock();
m_Owner=current;
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner)
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(m_Owner!=current)
	return;
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
Scheduler::ResumeTask(waiting);
}

VOID Mutex::Unlock(AccessMode)
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
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
Scheduler::ResumeTask(waiting);
}

VOID Mutex::Unlock(AccessPriority)
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner)
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(m_Owner!=current)
	return;
m_Owner->Unlock();
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
Scheduler::ResumeTask(waiting);
}


//================
// Common Private
//================

VOID Mutex::Yield(SpinLock& lock)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
Scheduler::ResumeTask(waiting);
lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner);
lock.Yield();
}

VOID Mutex::Yield(SpinLock& lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
auto parallel=Scheduler::RemoveParallelTask(m_Owner, current);
if(parallel)
	{
	m_Owner=parallel;
	}
else
	{
	auto waiting=m_Owner->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	m_Owner=waiting;
	Scheduler::ResumeTask(waiting);
	}
lock.Yield();
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
}

VOID Mutex::Yield(SpinLock& lock, AccessPriority priortiy)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
Scheduler::ResumeTask(waiting);
lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(m_Owner);
lock.Yield();
}

}