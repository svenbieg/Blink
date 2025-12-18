//============
// ReadLock.h
//============

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Concurrency/ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Read-Lock
//===========

class ReadLock: public ScopedLock
{
public:
	// Con-/Destructors
	inline ReadLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock(AccessMode::ReadOnly);
		}
	inline ~ReadLock()
		{
		if(m_Mutex)
			{
			m_Mutex->Unlock(AccessMode::ReadOnly);
			m_Mutex=nullptr;
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->Lock(AccessMode::ReadOnly); }
	inline VOID Release() { m_Mutex=nullptr; }
	inline BOOL TryLock()override { return m_Mutex->TryLock(AccessMode::ReadOnly); }
	inline VOID Unlock()override { m_Mutex->Unlock(AccessMode::ReadOnly); }

private:
	// Common
	inline VOID Yield(SpinLock& SchedulerLock)override { m_Mutex->Yield(SchedulerLock, AccessMode::ReadOnly); }
	Mutex* m_Mutex;
};

}