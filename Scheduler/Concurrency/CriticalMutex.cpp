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

VOID CriticalMutex::Lock()noexcept
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

VOID CriticalMutex::Lock(AccessMode)noexcept
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
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::PrioritySharing);
current->m_PriorityCount++;
Scheduler::OwnerList::Append(&m_Owner, current);
return true;
}

VOID CriticalMutex::Unlock()noexcept
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner) // Accessing heap early
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
INT resume_count=Mutex::Unlock(current);
if(resume_count<0) // Mutex was already unlocked
	return;
if(--current->m_PriorityCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
	auto waiting=Scheduler::s_Waiting.First();
	if(waiting)
		{
		if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Priority))
			resume_count=TypeHelper::Min(resume_count, 1);
		}
	}
if(resume_count)
	Scheduler::ResumeWaitingTasks(resume_count, true);
}

VOID CriticalMutex::Unlock(AccessMode)noexcept
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
INT resume_count=Mutex::Unlock(current, AccessMode::ReadOnly);
if(resume_count<0) // Mutex was already unlocked
	return;
if(--current->m_PriorityCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Priority);
	auto waiting=Scheduler::s_Waiting.First();
	if(waiting)
		{
		if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Priority))
			resume_count=TypeHelper::Min(resume_count, 1);
		}
	}
if(resume_count)
	Scheduler::ResumeWaitingTasks(resume_count, true);
}

}