//==============
// SpinLock.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

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


//================
// Common Private
//================

VOID SpinLock::Yield(SpinLock& lock)
{
Unlock();
lock.Yield();
Lock();
}

}