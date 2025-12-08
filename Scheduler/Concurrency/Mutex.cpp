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
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
AddWaitingTask(current);
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
assert(m_Owner==current);
if(m_Waiting)
	{
	m_Owner=m_Waiting;
	m_Waiting=m_Waiting->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	Scheduler::ResumeTask(core, current, m_Owner);
	}
else
	{
	m_Owner=nullptr;
	}
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
BOOL removed=Scheduler::RemoveParallelTask(&m_Owner, current);
assert(removed);
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owner)
	return;
if(m_Waiting)
	{
	m_Owner=m_Waiting;
	m_Waiting=m_Waiting->m_Waiting;
	m_Owner->m_Waiting=nullptr;
	Scheduler::ResumeTask(core, current, m_Owner);
	}
}


//==================
// Common Protected
//==================

VOID Mutex::AddWaitingTask(Task* task)
{
BOOL locked=FlagHelper::Get(task->m_Flags, TaskFlags::Locked);
auto current_ptr=&m_Waiting;
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	if(locked&&!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		{
		*current_ptr=task;
		task->m_Waiting=current;
		return;
		}
	current_ptr=&current->m_Waiting;
	}
*current_ptr=task;
}

VOID Mutex::AddWaitingTask(Task* task, AccessMode)
{
BOOL locked=FlagHelper::Get(task->m_Flags, TaskFlags::Locked);
auto current_ptr=&m_Waiting;
while(*current_ptr)
	{
	auto current=*current_ptr;
	assert(current!=task);
	if(locked&&!FlagHelper::Get(current->m_Flags, TaskFlags::Locked))
		{
		if(FlagHelper::Get(current->m_Flags, TaskFlags::Sharing))
			{
			Scheduler::AddParallelTask(current_ptr, task);
			}
		else
			{
			*current_ptr=task;
			task->m_Waiting=current;
			}
		return;
		}
	current_ptr=&current->m_Waiting;
	}
*current_ptr=task;
}


//================
// Common Private
//================

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
	Scheduler::ResumeTask(core, current, m_Owner);
	}
sched_lock.Yield();
if(!m_Owner)
	{
	m_Owner=current;
	return;
	}
AddWaitingTask(current);
Scheduler::SuspendCurrentTask(core, current);
sched_lock.Yield();
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
		Scheduler::ResumeTask(core, current, m_Owner);
		}
	}
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
sched_lock.Yield();
}

}