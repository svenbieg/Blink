//===================
// CriticalSection.h
//===================

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section


//=======
// Using
//=======

#pragma once

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
	~CriticalSection()=default;

	// Common
	VOID Lock();
	BOOL TryLock();
	VOID Unlock();
	VOID Yield();

private:
	// Common
	volatile UINT m_Core=0;
};

}