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
		m_Mutex->Lock(AccessMode::ReadOnly);
		}
	inline ~SharedLock()override
		{
		if(m_Mutex)
			{
			m_Mutex->Unlock(AccessMode::ReadOnly);
			m_Mutex=nullptr;
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->Lock(AccessMode::ReadOnly); }
	inline BOOL TryLock()override { return m_Mutex->TryLock(AccessMode::ReadOnly); }
	inline VOID Unlock()override { m_Mutex->Unlock(AccessMode::ReadOnly); }

private:
	// Common
	inline VOID Yield(SpinLock& Lock)override { m_Mutex->Yield(Lock, AccessMode::ReadOnly); }
};

}
