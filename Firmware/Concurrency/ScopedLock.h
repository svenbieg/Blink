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
	inline ScopedLock(Mutex& Mutex): m_Mutex(&Mutex)
		{
		m_Mutex->Lock();
		}
	virtual inline ~ScopedLock()
		{
		if(m_Mutex)
			m_Mutex->Unlock();
		}

	// Common
	virtual inline VOID Lock() { m_Mutex->Lock(); }
	inline VOID Release() { m_Mutex=nullptr; }
	virtual inline BOOL TryLock() { return m_Mutex->TryLock(); }
	virtual inline VOID Unlock() { m_Mutex->Unlock(); }

protected:
	// Con-/Destructors
	ScopedLock() {}

	// Common
	Mutex* m_Mutex;

private:
	// Common
	virtual inline VOID Yield(SpinLock& Lock) { m_Mutex->Yield(Lock); }
};

}
