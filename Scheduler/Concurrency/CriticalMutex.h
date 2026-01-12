//=================
// CriticalMutex.h
//=================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//================
// Critical-Mutex
//================

class CriticalMutex: public Mutex
{
public:
	// Con-/Destructors
	CriticalMutex()=default;
	CriticalMutex(CriticalMutex const&)=delete;

	// Common
	VOID Lock()override;
	VOID Lock(AccessMode)override;
	BOOL TryLock()override;
	BOOL TryLock(AccessMode)override;
	VOID Unlock()override;
	VOID Unlock(AccessMode)override;
};

}