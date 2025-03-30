//===========
// Mutex.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

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
if(Scheduler::AddWaitingTask(&m_Owner, current))
	Scheduler::SuspendCurrentTask(core, current);
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(Scheduler::AddWaitingTask(&m_Owner, current, AccessMode::ReadOnly))
	Scheduler::SuspendCurrentTask(core, current);
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

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(m_Owner!=current)
	return;
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!Scheduler::RemoveParallelTask(&m_Owner, current))
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owner)
	return;
auto waiting=current->m_Waiting;
current->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}


//================
// Common Private
//================

VOID Mutex::Yield(SpinLock& sched_lock)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
sched_lock.Yield();
if(Scheduler::AddWaitingTask(&m_Owner, current))
	{
	Scheduler::SuspendCurrentTask(core, current);
	sched_lock.Yield();
	}
}

VOID Mutex::Yield(SpinLock& sched_lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL removed=Scheduler::RemoveParallelTask(&m_Owner, current);
assert(removed);
if(!m_Owner)
	{
	auto waiting=current->m_Waiting;
	current->m_Waiting=nullptr;
	m_Owner=waiting;
	if(m_Owner)
		Scheduler::ResumeTask(core, current, m_Owner);
	}
sched_lock.Yield();
if(Scheduler::AddWaitingTask(&m_Owner, current, AccessMode::ReadOnly))
	{
	Scheduler::SuspendCurrentTask(core, current);
	sched_lock.Yield();
	}
}

}