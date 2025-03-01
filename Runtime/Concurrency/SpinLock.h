//============
// SpinLock.h
//============

#pragma once


//=======
// Using
//=======

#include "CriticalSection.h"
#include "ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Spin-Lock
//===========

class SpinLock: public ScopedLock
{
public:
	// Con-/Destructors
	SpinLock(CriticalSection& CriticalSection);
	~SpinLock();

	// Common
	inline VOID Lock()override { m_CriticalSection->Lock(); }
	inline BOOL TryLock()override { return m_CriticalSection->TryLock(); }
	inline VOID Unlock()override { m_CriticalSection->Unlock(); }
	inline VOID Yield() { m_CriticalSection->Yield(); }

private:
	// Common
	VOID Yield(SpinLock& Lock);
	CriticalSection* m_CriticalSection;
};

}