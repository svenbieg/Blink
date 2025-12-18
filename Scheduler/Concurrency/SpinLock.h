//============
// SpinLock.h
//============

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section


//=======
// Using
//=======

#pragma once

#include "Concurrency/CriticalSection.h"
#include "Concurrency/ScopedLock.h"


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
	SpinLock()=delete;
	SpinLock(SpinLock const&)=delete;
	SpinLock(CriticalSection& CriticalSection);
	~SpinLock();

	// Common
	inline VOID Lock()override { m_CriticalSection->Lock(); }
	inline BOOL TryLock()override { return m_CriticalSection->TryLock(); }
	inline VOID Unlock()override { m_CriticalSection->Unlock(); }
	inline VOID Yield() { m_CriticalSection->Yield(); }

private:
	// Common
	VOID Yield(SpinLock& SchedulerLock);
	CriticalSection* m_CriticalSection;
};

}