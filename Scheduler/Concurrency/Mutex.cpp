//===========
// Mutex.cpp
//===========

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#include "Mutex.h"


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

VOID Mutex::Lock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
LockInternal(core, current);
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
LockInternal(core, current, AccessMode::ReadOnly);
}

BOOL Mutex::TryLock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	return false;
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
m_Owner=current;
return true;
}

BOOL Mutex::TryLock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
if(m_Owner)
	{
	if(!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
		return false;
	}
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
Scheduler::AddParallelTask(&m_Owner, current);
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
UnlockInternal(core, current);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
UnlockInternal(core, current, AccessMode::ReadOnly);
}


//==================
// Common Protected
//==================

BOOL Mutex::LockInternal(UINT core, Task* current)
{
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
assert(m_Owner!=current);
AddWaitingTask(current);
Scheduler::SuspendCurrentTask(core, current);
return false;
}

BOOL Mutex::LockInternal(UINT core, Task* current, AccessMode)
{
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
if(FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	{
	if(!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Locked))
		{
		Scheduler::AddParallelTask(&m_Owner, current);
		return true;
		}
	}
AddWaitingTask(current, AccessMode::ReadOnly);
Scheduler::SuspendCurrentTask(core, current);
return false;
}

VOID Mutex::UnlockInternal(UINT core, Task* current)
{
if(m_Owner!=current)
	return;
m_Owner=m_Waiting;
if(m_Waiting)
	{
	m_Waiting=m_Waiting->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	Scheduler::WakeupTasks(m_Owner, Status::Success);
	}
}

VOID Mutex::UnlockInternal(UINT core, Task* current, AccessMode)
{
if(!Scheduler::RemoveParallelTask(&m_Owner, current))
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owner)
	return;
if(m_Waiting)
	{
	m_Owner=m_Waiting;
	m_Waiting=m_Waiting->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	Scheduler::WakeupTasks(m_Owner, Status::Success);
	}
}


//================
// Common Private
//================

VOID Mutex::AddWaitingTask(Task* task)
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

VOID Mutex::AddWaitingTask(Task* task, AccessMode)
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
				if(!current->m_Waiting)
					{
					Scheduler::AddParallelTask(current_ptr, task);
					return;
					}
				}
			}
		current_ptr=&current->m_Waiting;
		}
	}
*current_ptr=task;
}

VOID Mutex::Yield(SpinLock& sched_lock)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
if(m_Waiting)
	{
	m_Owner=m_Waiting;
	m_Waiting=m_Waiting->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	Scheduler::WakeupTasks(m_Owner, Status::Success);
	}
Scheduler::ResumeWaitingTask(core, current);
sched_lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
AddWaitingTask(current);
Scheduler::SuspendCurrentTask(core, current);
}

VOID Mutex::Yield(SpinLock& sched_lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL removed=Scheduler::RemoveParallelTask(&m_Owner, current);
assert(removed);
if(!m_Owner)
	{
	if(m_Waiting)
		{
		m_Owner=m_Waiting;
		m_Waiting=m_Waiting->m_Waiting;
		m_Owner->m_Waiting=nullptr;
		Scheduler::WakeupTasks(m_Owner, Status::Success);
		}
	}
Scheduler::ResumeWaitingTask(core, current);
sched_lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
if(FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	{
	Scheduler::AddParallelTask(&m_Owner, current);
	return;
	}
AddWaitingTask(current, AccessMode::ReadOnly);
Scheduler::SuspendCurrentTask(core, current);
}

}