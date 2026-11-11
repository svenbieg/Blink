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

BOOL ServiceMutex::Lock(Task* current)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(current);
}

BOOL ServiceMutex::Lock(Task* current, AccessMode)noexcept
{
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return Mutex::Lock(current, AccessMode::ReadOnly);
}

VOID ServiceMutex::Unlock(Task* current)noexcept
{
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
Mutex::Unlock(current);
}

VOID ServiceMutex::Unlock(Task* current, AccessMode)noexcept
{
if(--current->m_PriorityCount==0)
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
Mutex::Unlock(current, AccessMode::ReadOnly);
}

}