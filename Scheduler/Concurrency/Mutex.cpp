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

Mutex::~Mutex()noexcept
{
assert(!m_Owner);
assert(!m_Waiting);
}


//========
// Common
//========

VOID Mutex::Lock()noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
if(!current)
	return;
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
if(!this->Lock(core, current))
	Scheduler::Suspend(core, current);
}

VOID Mutex::Lock(AccessMode)noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
if(!this->Lock(core, current, AccessMode::ReadOnly))
	Scheduler::Suspend(core, current);
}

BOOL Mutex::TryLock()noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
return this->TryLock(core, current);
}

BOOL Mutex::TryLock(AccessMode)noexcept
{
// You can not use a Mutex in an ISR, You have to use a CriticalSection instead.
assert(!Interrupts::Active());
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
// You can only hold one ReadLock at a time.
assert(!FlagHelper::Get(current->m_Flags, TaskFlags::Sharing));
return this->TryLock(core, current, AccessMode::ReadOnly);
}

VOID Mutex::Unlock()noexcept
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
this->Unlock(current);
}

VOID Mutex::Unlock(AccessMode)noexcept
{
SpinLock lock(Scheduler::s_CriticalSection);
UINT core=Cpu::GetId();
auto current=Scheduler::s_CurrentTask[core];
this->Unlock(current, AccessMode::ReadOnly);
}


//==================
// Common Protected
//==================

BOOL Mutex::Lock(UINT core, Task* current)noexcept
{
if(!m_Owner)
	{
	m_Owner=current;
	return true;
	}
assert(m_Owner!=current); // Deadlock
Scheduler::WaitingList::Append(&m_Waiting, current);
return false;
}

BOOL Mutex::Lock(UINT core, Task* current, AccessMode)noexcept
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
Scheduler::WaitingList::Append(&m_Waiting, current);
return false;
}

VOID Mutex::ResumeWaitingTasks()noexcept
{
assert(m_Owner==nullptr);
auto resume=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
if(!resume)
	return;
m_Owner=resume;
Scheduler::Resume(resume);
if(!FlagHelper::Get(resume->m_Flags, TaskFlags::Sharing))
	return;
while(m_Waiting)
	{
	if(!FlagHelper::Get(m_Waiting->m_Flags, TaskFlags::Sharing))
		break;
	resume=Scheduler::WaitingList::RemoveFirst(&m_Waiting);
	Scheduler::OwnerList::Append(&m_Owner, resume);
	Scheduler::Resume(resume);
	}
}

BOOL Mutex::TryLock(UINT core, Task* current)noexcept
{
if(m_Owner)
	return false;
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
m_Owner=current;
return true;
}

BOOL Mutex::TryLock(UINT core, Task* current, AccessMode)noexcept
{
if(m_Owner)
	{
	if(!FlagHelper::Get(m_Owner->m_Flags, TaskFlags::Sharing))
		return false;
	if(m_Waiting)
		return false;
	}
FlagHelper::Set(current->m_Flags, TaskFlags::Sharing);
Scheduler::OwnerList::Append(&m_Owner, current);
return true;
}

VOID Mutex::Unlock(Task* current)noexcept
{
if(m_Owner!=current)
	return;
Scheduler::OwnerList::RemoveFirst(&m_Owner);
ResumeWaitingTasks();
}

VOID Mutex::Unlock(Task* current, AccessMode)noexcept
{
if(!Scheduler::OwnerList::TryRemove(&m_Owner, current))
	return;
FlagHelper::Clear(current->m_Flags, TaskFlags::Sharing);
if(!m_Owner)
	ResumeWaitingTasks();
}

}