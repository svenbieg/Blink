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

VOID CriticalMutex::Lock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current)
	return;
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!LockInternal(core, current))
	lock.Yield();
FlagHelper::Set(current->m_Flags, TaskFlags::Locked);
current->m_LockCount++;
}

VOID CriticalMutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!LockInternal(core, current, AccessMode::ReadOnly))
	lock.Yield();
FlagHelper::Set(current->m_Flags, TaskFlags::Locked);
current->m_LockCount++;
}

BOOL CriticalMutex::TryLock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owners)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owners=current;
FlagHelper::Set(current->m_Flags, TaskFlags::Locked);
current->m_LockCount++;
return true;
}

BOOL CriticalMutex::TryLock(AccessMode)
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
FlagHelper::Set(current->m_Flags, TaskFlags::LockedSharing);
current->m_LockCount++;
Scheduler::OwnerList::Append(&m_Owners, current);
return true;
}

VOID CriticalMutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owners)
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owners==current);
UnlockInternal(current);
if(--current->m_LockCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Locked);
	Scheduler::ResumeWaitingTask(core, current);
	}
}

VOID CriticalMutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
UnlockInternal(current, AccessMode::ReadOnly);
if(--current->m_LockCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Locked);
	Scheduler::ResumeWaitingTask(core, current);
	}
}

}