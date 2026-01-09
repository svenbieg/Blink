//===========
// Mutex.cpp
//===========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#include "Mutex.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "FlagHelper.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

Mutex::~Mutex()
{
assert(!m_Owners);
assert(!m_Waiting);
}


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
LockInternal(core, current);
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
LockInternal(core, current, AccessMode::ReadOnly);
}

BOOL Mutex::TryLock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owners)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owners=current;
return true;
}

BOOL Mutex::TryLock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owners)
	{
	if(!FlagHelper::Get(m_Owners->m_Flags, TaskFlags::Sharing))
		return false;
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
Scheduler::OwnerList::Append(&m_Owners, current);
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owners==current);
UnlockInternal(core, current);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
UnlockInternal(core, current, AccessMode::ReadOnly);
}


//==================
// Common Protected
//==================

BOOL Mutex::LockInternal(UINT core, Task* current)
{
if(!m_Owners)
	{
	m_Owners=current;
	return true;
	}
assert(m_Owners!=current);
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
Scheduler::SuspendCurrentTask(core, current);
return false;
}

BOOL Mutex::LockInternal(UINT core, Task* current, AccessMode)
{
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(!m_Owners)
	{
	m_Owners=current;
	return true;
	}
if(FlagHelper::Get(m_Owners->m_Flags, TaskFlags::Sharing))
	{
	if(!m_Waiting)
		{
		Scheduler::OwnerList::Append(&m_Owners, current);
		return true;
		}
	}
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
Scheduler::SuspendCurrentTask(core, current);
return false;
}

VOID Mutex::ResumeWaitingTask(UINT core, Task* current)
{
auto waiting=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
if(!waiting)
	return;
m_Owners=waiting;
FlagHelper::Clear(waiting->m_Flags, TaskFlags::Suspended);
Scheduler::s_Waiting.Insert(waiting, Task::Priority);
if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
	{
	while(m_Waiting)
		{
		if(!FlagHelper::Get(m_Waiting->m_Flags, TaskFlags::Sharing))
			break;
		auto resume=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
		FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
		Scheduler::OwnerList::Append(&m_Owners, resume);
		}
	}
Scheduler::ResumeWaitingTask(core, current, false);
}

VOID Mutex::UnlockInternal(UINT core, Task* current)
{
if(m_Owners!=current)
	return;
Scheduler::OwnerList::RemoveFirst(&m_Owners);
ResumeWaitingTask(core, current);
}

VOID Mutex::UnlockInternal(UINT core, Task* current, AccessMode)
{
BOOL removed=Scheduler::OwnerList::TryRemove(&m_Owners, current);
if(!removed)
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owners)
	return;
ResumeWaitingTask(core, current);
}


//================
// Common Private
//================

VOID Mutex::Yield(SpinLock& sched_lock)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owners==current);
Scheduler::OwnerList::RemoveFirst(&m_Owners);
ResumeWaitingTask(core, current);
sched_lock.Yield();
if(!m_Owners)
	{
	m_Owners=current;
	return;
	}
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
Scheduler::SuspendCurrentTask(core, current);
}

VOID Mutex::Yield(SpinLock& sched_lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL removed=Scheduler::OwnerList::TryRemove(&m_Owners, current);
assert(removed);
ResumeWaitingTask(core, current);
sched_lock.Yield();
if(!m_Owners)
	{
	m_Owners=current;
	return;
	}
if(FlagHelper::Get(m_Owners->m_Flags, TaskFlags::Sharing))
	{
	Scheduler::OwnerList::Append(&m_Owners, current);
	return;
	}
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
FlagHelper::Set(current->m_Flags, TaskFlags::Suspended);
Scheduler::SuspendCurrentTask(core, current);
}

}