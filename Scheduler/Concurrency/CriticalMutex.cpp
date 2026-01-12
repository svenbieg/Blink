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
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current) // Accessing heap early
	return;
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!Mutex::Lock(core, current))
	lock.Yield();
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
}

VOID CriticalMutex::Lock(AccessMode)
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!Mutex::Lock(core, current, AccessMode::ReadOnly))
	lock.Yield();
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
}

BOOL CriticalMutex::TryLock()
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owners)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owners=current;
FlagHelper::Set(current->m_Flags, TaskFlags::Priority);
current->m_PriorityCount++;
return true;
}

BOOL CriticalMutex::TryLock(AccessMode)
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owners)
	{
	if(!FlagHelper::Get(m_Owners->m_Flags, TaskFlags::Sharing))
		return false;
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::PrioritySharing);
current->m_PriorityCount++;
Scheduler::OwnerList::Append(&m_Owners, current);
return true;
}

VOID CriticalMutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owners) // Accessing heap early
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owners==current);
BOOL resume=Mutex::Unlock(current);
if(--current->m_PriorityCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
	auto waiting=Scheduler::s_Waiting.First();
	if(waiting)
		resume|=FlagHelper::Get(waiting->m_Flags, TaskFlags::Priority);
	}
if(resume)
	Scheduler::ResumeWaitingTask(core, current);
}

VOID CriticalMutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL resume=Mutex::Unlock(current, AccessMode::ReadOnly);
if(--current->m_PriorityCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
	auto waiting=Scheduler::s_Waiting.First();
	if(waiting)
		resume|=FlagHelper::Get(waiting->m_Flags, TaskFlags::Priority);
	}
if(resume)
	Scheduler::ResumeWaitingTask(core, current);
}

}