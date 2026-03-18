//===============
// WriteLock.cpp
//===============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#include "WriteLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

WriteLock::WriteLock(Mutex& mutex)noexcept:
m_Mutex(&mutex)
{
m_Mutex->Lock();
}

WriteLock::~WriteLock()noexcept
{
if(m_Mutex)
	{
	m_Mutex->Unlock();
	m_Mutex=nullptr;
	}
}


//========
// Common
//========

VOID WriteLock::Lock()noexcept
{
m_Mutex->Lock();
}

VOID WriteLock::Release()noexcept
{
m_Mutex=nullptr;
}

BOOL WriteLock::TryLock()noexcept
{
return m_Mutex->TryLock();
}

VOID WriteLock::Unlock()noexcept
{
m_Mutex->Unlock();
}


//================
// Common Private
//================

VOID WriteLock::Lock(UINT core, Task* current)noexcept
{
m_Mutex->Lock(core, current);
}

VOID WriteLock::Unlock(UINT core, Task* current)noexcept
{
m_Mutex->Unlock(current);
}

}