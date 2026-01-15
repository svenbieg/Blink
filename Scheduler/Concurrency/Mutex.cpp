//===========
// Mutex.cpp
//===========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#include "Mutex.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "FlagHelper.h"
#include "StatusHelper.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

Mutex::~Mutex()
{
assert(!m_Owner);
assert(!m_Waiting);
}


//========
// Common
//========

VOID Mutex::Lock()
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
Lock(core, current);
}

VOID Mutex::Lock(AccessMode)
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
Lock(core, current, AccessMode::ReadOnly);
}

BOOL Mutex::TryLock()
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
return true;
}

BOOL Mutex::TryLock(AccessMode)
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
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
Scheduler::OwnerList::Append(&m_Owner, current);
return true;
}

VOID Mutex::Unlock()
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
assert(m_Owner==current);
UINT resume_count=Unlock(current);
if(resume_count)
	Scheduler::ResumeWaitingTasks(resume_count, false);
}

VOID Mutex::Unlock(AccessMode)
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
UINT resume_count=Unlock(current, AccessMode::ReadOnly);
if(resume_count)
	Scheduler::ResumeWaitingTasks(resume_count, false);
}


//==================
// Common Protected
//==================

BOOL Mutex::Lock(UINT core, Task* current)
{
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
assert(m_Owner!=current); // Deadlock
Scheduler::SuspendCurrentTask(core, current);
Scheduler::WaitingList::Append(&m_Waiting, current);
return false;
}

BOOL Mutex::Lock(UINT core, Task* current, AccessMode)
{
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
if(FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
	{
	if(!m_Waiting)
		{
		Scheduler::OwnerList::Append(&m_Owner, current);
		return true;
		}
	}
Scheduler::SuspendCurrentTask(core, current);
Scheduler::WaitingList::Append(&m_Waiting, current);
return false;
}

UINT Mutex::Unlock(Task* current)
{
if(m_Owner!=current) // Maybe unlocked in lock-destructor
	return 0;
Scheduler::OwnerList::RemoveFirst(&m_Owner);
return WakeupWaitingTasks();
}

UINT Mutex::Unlock(Task* current, AccessMode)
{
BOOL removed=Scheduler::OwnerList::TryRemove(&m_Owner, current);
if(!removed) // Maybe unlocked in lock-destructor
	return 0;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(m_Owner)
	return 0;
return WakeupWaitingTasks();
}

UINT Mutex::WakeupWaitingTasks()
{
auto waiting=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
if(!waiting)
	return 0;
m_Owner=waiting;
FlagHelper::Clear(waiting->m_Flags, TaskFlags::Suspended);
Scheduler::s_Waiting.Insert(waiting, Task::Priority);
UINT count=1;
if(FlagHelper::Get(waiting->m_Flags, TaskFlags::Sharing))
	{
	while(m_Waiting)
		{
		if(!FlagHelper::Get(m_Waiting->m_Flags, TaskFlags::Sharing))
			break;
		auto resume=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
		Scheduler::OwnerList::Insert(&m_Owner, resume, Task::Priority);
		FlagHelper::Clear(resume->m_Flags, TaskFlags::Suspended);
		Scheduler::s_Waiting.Insert(waiting, Task::Priority);
		count++;
		}
	}
return count;
}

}