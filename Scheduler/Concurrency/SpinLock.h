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
	SpinLock(CriticalSection& CriticalSection)noexcept;
	SpinLock(SpinLock const&)=delete;
	~SpinLock()noexcept;

	// Common
	VOID Lock()noexcept;
	BOOL TryLock()noexcept;
	VOID Unlock()noexcept;
	VOID Yield()noexcept;

private:
	// Common
	CriticalSection* m_CriticalSection;
};

}