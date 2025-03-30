//=============
// WriteLock.h
//=============

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


//============
// Write-Lock
//============

class WriteLock: public ScopedLock
{
public:
	// Con-/Destructors
	inline WriteLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock();
		}
	inline ~WriteLock()
		{
		if(m_Mutex)
			{
			m_Mutex->Unlock();
			m_Mutex=nullptr;
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->Lock(); }
	inline VOID Release() { m_Mutex=nullptr; }
	inline BOOL TryLock()override { return m_Mutex->TryLock(); }
	inline VOID Unlock()override { m_Mutex->Unlock(); }

private:
	// Common
	inline VOID Yield(SpinLock& SchedulerLock)override { m_Mutex->Yield(SchedulerLock); }
	Mutex* m_Mutex;
};

}