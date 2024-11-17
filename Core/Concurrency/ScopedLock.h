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
	ScopedLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock();
		}
	virtual ~ScopedLock()
		{
		if(m_Mutex)
			m_Mutex->Unlock();
		}

	// Common
	virtual inline VOID Lock() { m_Mutex->Lock(); }
	inline VOID Release() { m_Mutex=nullptr; }
	virtual inline BOOL TryLock() { return m_Mutex->TryLock(); }
	inline VOID Unlock() { m_Mutex->Unlock(); }

protected:
	// Con-/Destructors
	ScopedLock() {}

	// Common
	Mutex* m_Mutex;
};

}
