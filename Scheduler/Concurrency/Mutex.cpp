//===========
// Mutex.cpp
//===========

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

VOID Mutex::Lock()
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(AddWaitingTask(current))
	Scheduler::SuspendCurrentTask(core, current);
}

VOID Mutex::Lock(AccessMode)
{
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(AddWaitingTask(current, AccessMode::ReadOnly))
	Scheduler::SuspendCurrentTask(core, current);
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
if(m_Owner&&!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	return false;
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
if(m_Owner!=current)
	return;
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!Scheduler::RemoveParallelTask(&m_Owner, current))
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owner)
	return;
auto waiting=current->m_Waiting;
current->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
}


//==================
// Common Protected
//==================

BOOL Mutex::AddWaitingTask(Task* task)
{
auto current_ptr=&m_Owner;
BOOL suspend=false;
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
			{
			task->m_Waiting=current;
			*current_ptr=task;
			return true;
			}
		suspend=true;
		current_ptr=&current->m_Waiting;
		}
	}
else
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		suspend=true;
		current_ptr=&current->m_Waiting;
		}
	}
*current_ptr=task;
return suspend;
}

BOOL Mutex::AddWaitingTask(Task* task, AccessMode)
{
auto current_ptr=&m_Owner;
BOOL suspend=false;
if(FlagHelper::Get(task->m_Flags, TaskFlags::Locked))
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		auto waiting=current->m_Waiting;
		if(waiting)
			{
			if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
				{
				Scheduler::AddParallelTask(&current->m_Waiting, task);
				return true;
				}
			if(!FlagHelper::Get(waiting->m_Flags, TaskFlags::Locked))
				{
				if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
					{
					Scheduler::AddParallelTask(current_ptr, task);
					return suspend;
					}
				current->m_Waiting=task;
				task->m_Waiting=waiting;
				return true;
				}
			suspend=true;
			current_ptr=&current->m_Waiting;
			continue;
			}
		if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
			{
			Scheduler::AddParallelTask(current_ptr, task);
			return suspend;
			}
		current->m_Waiting=task;
		return true;
		}
	}
else
	{
	while(*current_ptr)
		{
		auto current=*current_ptr;
		assert(current!=task);
		if(current->m_Waiting)
			{
			suspend=true;
			current_ptr=&current->m_Waiting;
			continue;
			}
		if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
			{
			Scheduler::AddParallelTask(current_ptr, task);
			return suspend;
			}
		current->m_Waiting=task;
		return true;
		}
	}
*current_ptr=task;
return suspend;
}


//================
// Common Private
//================

VOID Mutex::Yield(SpinLock& sched_lock)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
auto waiting=m_Owner->m_Waiting;
m_Owner->m_Waiting=nullptr;
m_Owner=waiting;
if(m_Owner)
	Scheduler::ResumeTask(core, current, m_Owner);
sched_lock.Yield();
if(AddWaitingTask(current))
	{
	Scheduler::SuspendCurrentTask(core, current);
	sched_lock.Yield();
	}
}

VOID Mutex::Yield(SpinLock& sched_lock, AccessMode access)
{
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL removed=Scheduler::RemoveParallelTask(&m_Owner, current);
assert(removed);
if(!m_Owner)
	{
	auto waiting=current->m_Waiting;
	current->m_Waiting=nullptr;
	m_Owner=waiting;
	if(m_Owner)
		Scheduler::ResumeTask(core, current, m_Owner);
	}
sched_lock.Yield();
if(AddWaitingTask(current, AccessMode::ReadOnly))
	{
	Scheduler::SuspendCurrentTask(core, current);
	sched_lock.Yield();
	}
}

}