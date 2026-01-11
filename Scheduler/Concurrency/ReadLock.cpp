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
m_Mutex(&mutex)
{
m_Mutex->Lock(AccessMode::ReadOnly);
}

ReadLock::~ReadLock()
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

VOID ReadLock::Lock()
{
m_Mutex->Lock(AccessMode::ReadOnly);
}

VOID ReadLock::Release()
{
m_Mutex=nullptr;
}

BOOL ReadLock::TryLock()
{
return m_Mutex->TryLock(AccessMode::ReadOnly);
}

VOID ReadLock::Unlock()
{
m_Mutex->Unlock(AccessMode::ReadOnly);
}


//================
// Common Private
//================

VOID ReadLock::Lock(UINT core, Task* current)
{
m_Mutex->Lock(core, current, AccessMode::ReadOnly);
}

VOID ReadLock::Unlock(UINT core, Task* current)
{
m_Mutex->Unlock(current, AccessMode::ReadOnly);
}

}