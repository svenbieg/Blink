//===================
// CriticalMutex.cpp
//===================

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization

#include "CriticalMutex.h"


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
FlagHelper::Set(current->m_Flags, TaskFlags::LockedSharing);
current->m_LockCount++;
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
if(m_Owner)
	return false;
if(!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
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
assert(m_Owner==current);
UnlockInternal(core, current);
if(--m_Owner->m_LockCount==0)
	{
	FlagHelper::Clear(m_Owner->m_Flags, TaskFlags::Locked);
	Scheduler::ResumeWaitingTask(core, current);
	}
}

VOID CriticalMutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
UnlockInternal(core, current, AccessMode::ReadOnly);
if(--current->m_LockCount==0)
	{
	FlagHelper::Clear(m_Owner->m_Flags, TaskFlags::Locked);
	Scheduler::ResumeWaitingTask(core, current);
	}
}


//================
// Common Private
//================

VOID CriticalMutex::AddWaitingTask(Task* task)
{
auto current_ptr=&m_Waiting;
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			*current_ptr=task;
			task->m_Waiting=current;
			return;
			}
		current_ptr=&current->m_Waiting;
		}
	}
else
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		current_ptr=&current->m_Waiting;
		}
	}
*current_ptr=task;
}

VOID CriticalMutex::AddWaitingTask(Task* task, AccessMode)
{
auto current_ptr=&m_Waiting;
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			*current_ptr=task;
			task->m_Waiting=current;
			return;
			}
		if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
			{
			Scheduler::AddParallelTask(current_ptr, task);
			return;
			}
		current_ptr=&current->m_Waiting;
		}
	}
else
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
				{
				Scheduler::AddParallelTask(current_ptr, task);
				return;
				}
			}
		current_ptr=&current->m_Waiting;
		}
	}
*current_ptr=task;
}

}