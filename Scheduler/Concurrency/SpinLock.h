//============
// SpinLock.h
//============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section

#pragma once


//=======
// Using
//=======

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
	SpinLock(SpinLock const&)=delete;
	SpinLock(CriticalSection& CriticalSection);
	~SpinLock();

	// Common
	VOID Lock()override;
	BOOL TryLock()override;
	VOID Unlock()override;
	VOID Yield();

private:
	// Common
	VOID Yield(SpinLock& SchedulerLock)override;
	CriticalSection* m_CriticalSection;
};

}