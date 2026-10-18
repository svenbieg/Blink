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

ReadLock::ReadLock(Mutex& mutex):
m_Locked(true),
m_Mutex(&mutex)
{
m_Mutex->Lock(AccessMode::ReadOnly);
}

ReadLock::~ReadLock()noexcept
{
if(m_Locked)
	{
	m_Mutex->Unlock(AccessMode::ReadOnly);
	m_Locked=false;
	m_Mutex=nullptr;
	}
}


//========
// Common
//========

VOID ReadLock::Lock()
{
assert(!m_Locked);
m_Mutex->Lock(AccessMode::ReadOnly);
m_Locked=true;
}

VOID ReadLock::Release()noexcept
{
m_Locked=false;
m_Mutex=nullptr;
}

BOOL ReadLock::TryLock()noexcept
{
assert(!m_Locked);
if(m_Mutex->TryLock(AccessMode::ReadOnly))
	{
	m_Locked=true;
	return true;
	}
return false;
}

VOID ReadLock::Unlock()noexcept
{
assert(m_Locked);
m_Mutex->Unlock(AccessMode::ReadOnly);
m_Locked=false;
}

}