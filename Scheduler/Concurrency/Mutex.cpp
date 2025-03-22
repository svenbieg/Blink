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
#include "FlagHelper.h"

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
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(&m_Owner, core, current);
lock.Yield();
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
auto waiting=m_Owner;
while(waiting->m_Waiting)
	waiting=waiting->m_Waiting;
if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
	{
	Scheduler::AddParallelTask(&waiting, current);
	if(waiting==m_Owner)
		return;
	Scheduler::SuspendCurrentTask(nullptr, core, current);
	}
else
	{
	Scheduler::SuspendCurrentTask(&waiting, core, current);
	}
lock.Yield();
}

VOID Mutex::Lock(AccessPriority)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(current)
	{
	assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
	current->Lock();
	}
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(&m_Owner, core, current);
lock.Yield();
}

BOOL Mutex::TryLock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owner=current;
return true;
}

BOOL Mutex::TryLock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner&&!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
Scheduler::AddParallelTask(&m_Owner, current);
return true;
}

BOOL Mutex::TryLock(AccessPriority)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
current->Lock();
m_Owner=current;
return true;
}

VOID Mutex::Unlock()
{
assert(!Interrupts::Active());
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
if(waiting)
	Scheduler::ResumeTask(waiting);
}

VOID Mutex::Unlock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
auto owner=m_Owner;
Scheduler::RemoveParallelTask(&m_Owner, current);
if(m_Owner)
	return;
auto waiting=owner->m_Waiting;
owner->m_Waiting=nullptr;
m_Owner=waiting;
if(waiting)
	Scheduler::ResumeTask(waiting);
}

VOID Mutex::Unlock(AccessPriority)
{
assert(!Interrupts::Active());
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
if(waiting)
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
if(waiting)
	Scheduler::ResumeTask(waiting);
lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(&m_Owner, core, current);
lock.Yield();
}

VOID Mutex::Yield(SpinLock& lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
auto owner=m_Owner;
Scheduler::RemoveParallelTask(&m_Owner, current);
if(!m_Owner)
	{
	auto waiting=owner->m_Waiting;
	owner->m_Waiting=nullptr;
	m_Owner=waiting;
	if(waiting)
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
if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
	{
	Scheduler::AddParallelTask(&waiting, current);
	if(waiting==m_Owner)
		return;
	Scheduler::SuspendCurrentTask(nullptr, core, current);
	}
else
	{
	Scheduler::SuspendCurrentTask(&waiting, core, current);
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
if(waiting)
	Scheduler::ResumeTask(waiting);
lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
Scheduler::SuspendCurrentTask(&m_Owner, core, current);
lock.Yield();
}

}