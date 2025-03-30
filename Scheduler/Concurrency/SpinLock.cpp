//==============
// SpinLock.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"


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


//================
// Common Private
//================

VOID SpinLock::Yield(SpinLock& sched_lock)
{
m_CriticalSection->Unlock();
sched_lock.Yield();
m_CriticalSection->Lock();
}

}