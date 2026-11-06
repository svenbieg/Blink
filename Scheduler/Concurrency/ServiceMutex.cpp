//==================
// ServiceMutex.cpp
//==================

// A mutex raising priority to prevent suspension.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization

#include "ServiceMutex.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "FlagHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Common Protected
//==================

BOOL ServiceMutex::Lock(UINT core, Task* current)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(core, current);
}

BOOL ServiceMutex::Lock(UINT core, Task* current, AccessMode)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(core, current, AccessMode::ReadOnly);
}

VOID ServiceMutex::Unlock(Task* current)noexcept
{
assert(m_Owner==current);
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
Scheduler::OwnerList::RemoveFirst(&m_Owner);
Mutex::ResumeWaitingTasks();
}

VOID ServiceMutex::Unlock(Task* current, AccessMode)noexcept
{
Scheduler::OwnerList::Remove(&m_Owner, current);
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	Mutex::ResumeWaitingTasks();
}

}