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
#include "StatusHelper.h"

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
Lock(core, current);
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
Lock(core, current, AccessMode::ReadOnly);
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
if(Unlock(current))
	Scheduler::ResumeWaitingTask(core, current);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(Unlock(current, AccessMode::ReadOnly))
	Scheduler::ResumeWaitingTask(core, current);
}


//==================
// Common Protected
//==================

BOOL Mutex::Lock(UINT core, Task* current)
{
if(!m_Owners)
	{
	m_Owners=current;
	return true;
	}
assert(m_Owners!=current);
Scheduler::SuspendCurrentTask(core, current);
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
return false;
}

BOOL Mutex::Lock(UINT core, Task* current, AccessMode)
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
Scheduler::SuspendCurrentTask(core, current);
Scheduler::WaitingList::Insert(&m_Waiting, current, Task::Priority);
return false;
}

BOOL Mutex::Unlock(Task* current)
{
if(m_Owners!=current)
	return false;
Scheduler::OwnerList::RemoveFirst(&m_Owners);
return WakeupWaitingTasks();
}

BOOL Mutex::Unlock(Task* current, AccessMode)
{
BOOL removed=Scheduler::OwnerList::TryRemove(&m_Owners, current);
if(!removed)
	return false;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owners)
	return false;
return WakeupWaitingTasks();
}

BOOL Mutex::WakeupWaitingTasks()
{
assert(m_Owners==nullptr);
auto waiting=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
if(!waiting)
	return false;
m_Owners=waiting;
FlagHelper::Clear(waiting->m_Flags, TaskFlags::Suspended);
Scheduler::s_Waiting.Insert(waiting, Task::Prepend);
if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
	{
	while(m_Waiting)
		{
		if(!FlagHelper::Get(m_Waiting->m_Flags, TaskFlags::Sharing))
			break;
		auto resume=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
		Scheduler::OwnerList::Append(&m_Owners, resume);
		FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
		Scheduler::s_Waiting.Insert(waiting, Task::Prepend);
		}
	}
return true;
}

}