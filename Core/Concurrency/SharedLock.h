//==============
// SharedLock.h
//==============

#pragma once


//=======
// Using
//=======

#include "ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//=============
// Shared-Lock
//=============

class SharedLock: public ScopedLock
{
public:
	// Con-/Destructors
	inline SharedLock(Mutex& Mutex)
		{
		m_Mutex=&Mutex;
		m_Mutex->LockShared();
		}
	inline ~SharedLock()override
		{
		if(m_Mutex)
			{
			m_Mutex->UnlockShared();
			m_Mutex=nullptr; // Compiler-bug: ~ScopedLock() is overridden but called
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->LockShared(); }
	inline BOOL TryLock()override { return m_Mutex->TryLockShared(); }
	inline VOID Unlock()override { m_Mutex->UnlockShared(); }

private:
	// Common
	Mutex* m_Mutex;
};

}
