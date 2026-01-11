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
m_Mutex(&mutex)
{
m_Mutex->Lock();
}

WriteLock::~WriteLock()
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

VOID WriteLock::Lock()
{
m_Mutex->Lock();
}

VOID WriteLock::Release()
{
m_Mutex=nullptr;
}

BOOL WriteLock::TryLock()
{
return m_Mutex->TryLock();
}

VOID WriteLock::Unlock()
{
m_Mutex->Unlock();
}


//================
// Common Private
//================

BOOL WriteLock::Lock(UINT core, Task* current)
{
return m_Mutex->Lock(core, current);
}

VOID WriteLock::Unlock(UINT core, Task* current)
{
m_Mutex->Unlock(current);
}

}