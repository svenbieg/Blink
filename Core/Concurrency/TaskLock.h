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

	// Common
	inline VOID Lock()override { m_Mutex->LockBlocking(); }
	inline BOOL TryLock()override { return m_Mutex->TryLockBlocking(); }
};

}
