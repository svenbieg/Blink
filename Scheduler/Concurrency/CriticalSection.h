//===================
// CriticalSection.h
//===================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class SpinLock;


//==================
// Critical-Section
//==================

class CriticalSection
{
public:
	// Friends
	friend SpinLock;

	// Con-/Destructors
	CriticalSection(): m_Core(0) {}
	CriticalSection(CriticalSection const&)=delete;

	// Common
	VOID Lock()noexcept;
	BOOL TryLock()noexcept;
	VOID Unlock()noexcept;
	VOID Yield()noexcept;

private:
	// Common
	VOID Unlock(UINT Core);
	volatile UINT m_Core;
};

}