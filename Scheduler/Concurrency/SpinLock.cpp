//==============
// SpinLock.cpp
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section

#include "SpinLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

SpinLock::SpinLock(CriticalSection& critical)noexcept:
m_CriticalSection(&critical)
{
m_CriticalSection->Lock();
}

SpinLock::~SpinLock()noexcept
{
if(m_CriticalSection)
	{
	m_CriticalSection->Unlock();
	m_CriticalSection=nullptr;
	}
}


//========
// Common
//========

VOID SpinLock::Lock()noexcept
{
m_CriticalSection->Lock();
}

BOOL SpinLock::TryLock()noexcept
{
return m_CriticalSection->TryLock();
}

VOID SpinLock::Unlock()noexcept
{
m_CriticalSection->Unlock();
}

VOID SpinLock::Yield()noexcept
{
m_CriticalSection->Yield();
}


//================
// Common Private
//================

VOID SpinLock::Lock(UINT core, Task* current)noexcept
{
m_CriticalSection->Lock();
}

VOID SpinLock::Unlock(UINT core, Task* current)noexcept
{
m_CriticalSection->Unlock();
}

}