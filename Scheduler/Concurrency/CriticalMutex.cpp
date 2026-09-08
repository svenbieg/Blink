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


//==================
// Common Protected
//==================

BOOL CriticalMutex::Lock(UINT core, Task* current)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(core, current);
}

BOOL CriticalMutex::Lock(UINT core, Task* current, AccessMode)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(core, current, AccessMode::ReadOnly);
}

BOOL CriticalMutex::TryLock(UINT core, Task* current)noexcept
{
if(!Mutex::TryLock(core, current))
	return false;
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return true;
}

BOOL CriticalMutex::TryLock(UINT core, Task* current, AccessMode)noexcept
{
if(!Mutex::TryLock(core, current, AccessMode::ReadOnly))
	return false;
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return true;
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