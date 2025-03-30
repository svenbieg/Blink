//===================
// CriticalMutex.cpp
//===================

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

VOID CriticalMutex::Lock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current)
	return;
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Locked);
current->m_LockCount++;
if(Scheduler::AddWaitingTask(&m_Owner, current))
	Scheduler::SuspendCurrentTask(core, current);
}

VOID CriticalMutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::LockedSharing);
current->m_LockCount++;
if(Scheduler::AddWaitingTask(&m_Owner, current, AccessMode::ReadOnly))
	Scheduler::SuspendCurrentTask(core, current);
}

BOOL CriticalMutex::TryLock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Locked);
current->m_LockCount++;
m_Owner=current;
return true;
}

BOOL CriticalMutex::TryLock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner&&!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::LockedSharing);
current->m_LockCount++;
Scheduler::AddParallelTask(&m_Owner, current);
return true;
}

VOID CriticalMutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
if(!m_Owner)
	return;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(m_Owner!=current)
	return;
if(--m_Owner->m_LockCount==0)
	FlagHelper::Clear(m_Owner->m_Flags, TaskFlags::Locked);
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}

VOID CriticalMutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!Scheduler::RemoveParallelTask(&m_Owner, current))
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(--current->m_LockCount==0)
	{
	FlagHelper::Clear(current->m_Flags, TaskFlags::Locked);
	if(m_Owner)
		{
		Scheduler::SwitchCurrentTask(core, current);
		return;
		}
	}
auto waiting=current->m_Waiting;
current->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}

}