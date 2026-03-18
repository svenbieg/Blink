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
	SpinLock(CriticalSection& CriticalSection)noexcept;
	SpinLock(SpinLock const&)=delete;
	~SpinLock()noexcept;

	// Common
	VOID Lock()noexcept override;
	BOOL TryLock()noexcept override;
	VOID Unlock()noexcept override;
	VOID Yield()noexcept;

private:
	// Common
	VOID Lock(UINT Core, Task* Current)noexcept override;
	VOID Unlock(UINT Core, Task* Current)noexcept override;
	CriticalSection* m_CriticalSection;
};

}