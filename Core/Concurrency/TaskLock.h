//============
// TaskLock.h
//============

#pragma once


//=======
// Using
//=======

#include "ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Task-Lock
//===========

class TaskLock: public ScopedLock
{
public:
	// Con-/Destructors
	inline TaskLock(Mutex& Mutex)
		{
		m_Mutex=&Mutex;
		m_Mutex->LockBlocking();
		}
	inline ~TaskLock()override
		{
		if(m_Mutex)
			{
			m_Mutex->UnlockBlocking();
			m_Mutex=nullptr; // Compiler-bug: ~ScopedLock() is overridden but called
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->LockBlocking(); }
	inline BOOL TryLock()override { return m_Mutex->TryLockBlocking(); }
	inline VOID Unlock()override { m_Mutex->UnlockBlocking(); }
};

}
