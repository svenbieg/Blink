//============
// SpinLock.h
//============

#pragma once


//=======
// Using
//=======

#include "CriticalSection.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Spin-Lock
//===========

class SpinLock
{
public:
	// Con-/Destructors
	inline SpinLock(CriticalSection& CriticalSection):
		m_CriticalSection(&CriticalSection)
		{
		Lock();
		}
	inline ~SpinLock() { Unlock(); }

	// Common
	inline VOID Lock() { m_CriticalSection->Enter(); }
	inline VOID Unlock() { m_CriticalSection->Leave(); }
	inline VOID Yield() { Unlock(); Lock(); }

private:
	// Common
	CriticalSection* m_CriticalSection;
};

}
