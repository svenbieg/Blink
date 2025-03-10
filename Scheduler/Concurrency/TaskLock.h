//============
// TaskLock.h
//============

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
// Task-Lock
//===========

class TaskLock: public ScopedLock
{
public:
	// Con-/Destructors
	inline TaskLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock(AccessPriority::High);
		}
	inline ~TaskLock()override
		{
		if(m_Mutex)
			{
			m_Mutex->Unlock(AccessPriority::High);
			m_Mutex=nullptr;
			}
		}

	// Common
	inline VOID Lock()override { m_Mutex->Lock(AccessPriority::High); }
	inline BOOL TryLock()override { return m_Mutex->TryLock(AccessPriority::High); }
	inline VOID Unlock()override { m_Mutex->Unlock(AccessPriority::High); }

private:
	// Common
	inline VOID Yield(SpinLock& Lock)override { m_Mutex->Yield(Lock, AccessPriority::High); }
	Mutex* m_Mutex;
};

}