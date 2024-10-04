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
	inline TaskLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock(true);
		}

	// Common
	inline VOID Lock()override { m_Mutex->Lock(true); }
	inline BOOL TryLock()override { return m_Mutex->TryLock(true); }

private:
	// Common
	Mutex* m_Mutex;
};

}
