//==============
// ScopedLock.h
//==============

#pragma once


//=======
// Using
//=======

#include "Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//=============
// Scoped Lock
//=============

class ScopedLock
{
public:
	// Friends
	friend class Signal;

	// Con-/Destructors
	inline ScopedLock(Mutex& Mutex): m_Mutex(m_Mutex) {}
	inline ~ScopedLock()
		{
		if(m_Mutex)
			m_Mutex->Unlock();
		}

	// Common
	inline VOID Lock() { m_Mutex->Lock(); }
	inline VOID Release() { m_Mutex=nullptr; }
	inline VOID Unlock() { m_Mutex->Unlock(); }

private:
	// Common
	Mutex* m_Mutex;
};

}
