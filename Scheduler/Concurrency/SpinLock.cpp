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

SpinLock::SpinLock(CriticalSection& critical):
m_CriticalSection(&critical)
{
m_CriticalSection->Lock();
}

SpinLock::~SpinLock()
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

VOID SpinLock::Lock()
{
m_CriticalSection->Lock();
}

BOOL SpinLock::TryLock()
{
return m_CriticalSection->TryLock();
}

VOID SpinLock::Unlock()
{
m_CriticalSection->Unlock();
}

VOID SpinLock::Yield()
{
m_CriticalSection->Yield();
}


//================
// Common Private
//================

VOID SpinLock::Yield(SpinLock& sched_lock)
{
m_CriticalSection->Unlock();
sched_lock.m_CriticalSection->Yield();
m_CriticalSection->Lock();
}

}