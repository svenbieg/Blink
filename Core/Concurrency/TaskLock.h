//============
// TaskLock.h
//============

#pragma once


//=======
// Using
//=======

#include "Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Task-Lock
//===========

class TaskLock
{
public:
	// Con-/Destructors
	inline TaskLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock(true);
		}
	inline ~TaskLock()
		{
		if(m_Mutex)
			m_Mutex->Unlock();
		}

	// Common
	inline VOID Lock() { m_Mutex->Lock(true); }
	inline VOID Release() { m_Mutex=nullptr; }
	inline BOOL TryLock() { return m_Mutex->TryLock(true); }
	inline VOID Unlock() { m_Mutex->Unlock(); }

private:
	// Common
	Mutex* m_Mutex;
};

}
