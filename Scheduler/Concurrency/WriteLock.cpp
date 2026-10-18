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

WriteLock::WriteLock(Mutex& mutex):
m_Locked(true),
m_Mutex(&mutex)
{
m_Mutex->Lock();
}

WriteLock::~WriteLock()noexcept
{
if(m_Locked)
	{
	m_Mutex->Unlock();
	m_Locked=false;
	m_Mutex=nullptr;
	}
}


//========
// Common
//========

VOID WriteLock::Lock()
{
assert(!m_Locked);
m_Mutex->Lock();
m_Locked=true;
}

VOID WriteLock::Release()noexcept
{
m_Locked=false;
m_Mutex=nullptr;
}

BOOL WriteLock::TryLock()noexcept
{
assert(!m_Locked);
if(m_Mutex->TryLock())
	{
	m_Locked=true;
	return true;
	}
return false;
}

VOID WriteLock::Unlock()noexcept
{
assert(m_Locked);
m_Mutex->Unlock();
m_Locked=false;
}


//================
// Common Private
//================

BOOL WriteLock::Lock(UINT core, Task* current)noexcept
{
assert(!m_Locked);
m_Locked=true;
return m_Mutex->Lock(core, current);
}

VOID WriteLock::Unlock(UINT core, Task* current)noexcept
{
assert(m_Locked);
m_Locked=false;
m_Mutex->Unlock(current);
}

}