//===================
// CriticalMutex.cpp
//===================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization

#include "CriticalMutex.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
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

BOOL CriticalMutex::TryLock()noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owner=current;
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return true;
}

BOOL CriticalMutex::TryLock(AccessMode)noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	{
	if(!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
		return false;
	if(m_Waiting)
		return false;
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
Scheduler::OwnerList::Append(&m_Owner, current);
return true;
}


//==================
// Common Protected
//==================

VOID CriticalMutex::Lock(UINT core, Task* current)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
Mutex::Lock(core, current);
}

VOID CriticalMutex::Lock(UINT core, Task* current, AccessMode)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
Mutex::Lock(core, current, AccessMode::ReadOnly);
}

VOID CriticalMutex::Unlock(Task* current)noexcept
{
if(m_Owner!=current)
	return;
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
Scheduler::OwnerList::RemoveFirst(&m_Owner);
Mutex::ResumeWaitingTasks();
}

VOID CriticalMutex::Unlock(Task* current, AccessMode)noexcept
{
if(!Scheduler::OwnerList::TryRemove(&m_Owner, current))
	return;
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	Mutex::ResumeWaitingTasks();
}

}