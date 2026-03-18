//==============
// ReadLock.cpp
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#include "ReadLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

ReadLock::ReadLock(Mutex& mutex)noexcept:
m_Mutex(&mutex)
{
m_Mutex->Lock(AccessMode::ReadOnly);
}

ReadLock::~ReadLock()noexcept
{
if(m_Mutex)
	{
	m_Mutex->Unlock(AccessMode::ReadOnly);
	m_Mutex=nullptr;
	}
}


//========
// Common
//========

VOID ReadLock::Lock()noexcept
{
m_Mutex->Lock(AccessMode::ReadOnly);
}

VOID ReadLock::Release()noexcept
{
m_Mutex=nullptr;
}

BOOL ReadLock::TryLock()noexcept
{
return m_Mutex->TryLock(AccessMode::ReadOnly);
}

VOID ReadLock::Unlock()noexcept
{
m_Mutex->Unlock(AccessMode::ReadOnly);
}


//================
// Common Private
//================

VOID ReadLock::Lock(UINT core, Task* current)noexcept
{
m_Mutex->Lock(core, current, AccessMode::ReadOnly);
}

VOID ReadLock::Unlock(UINT core, Task* current)noexcept
{
m_Mutex->Unlock(current, AccessMode::ReadOnly);
}

}