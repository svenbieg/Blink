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


//==================
// Critical-Section
//==================

class CriticalSection
{
public:
	// Con-/Destructors
	CriticalSection()=default;
	CriticalSection(CriticalSection const&)=delete;

	// Common
	VOID Lock()noexcept;
	BOOL TryLock()noexcept;
	VOID Unlock()noexcept;
	VOID Yield()noexcept;

private:
	// Common
	volatile UINT m_Core=0;
};

}