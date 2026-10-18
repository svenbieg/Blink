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
m_CriticalSection(&critical),
m_Locked(true)
{
m_CriticalSection->Lock();
}

SpinLock::~SpinLock()noexcept
{
if(m_Locked)
	{
	m_CriticalSection->Unlock();
	m_Locked=false;
	}
m_CriticalSection=nullptr;
}


//========
// Common
//========

VOID SpinLock::Lock()noexcept
{
assert(!m_Locked);
m_CriticalSection->Lock();
m_Locked=true;
}

BOOL SpinLock::TryLock()noexcept
{
assert(!m_Locked);
if(m_CriticalSection->TryLock())
	{
	m_Locked=true;
	return true;
	}
return false;
}

VOID SpinLock::Unlock()noexcept
{
assert(m_Locked);
m_CriticalSection->Unlock();
m_Locked=false;
}

}