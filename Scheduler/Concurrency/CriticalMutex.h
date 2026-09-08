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

protected:
	// Common
	BOOL Lock(UINT Core, Task* Current)noexcept override;
	BOOL Lock(UINT Core, Task* Current, AccessMode)noexcept override;
	BOOL TryLock(UINT Core, Task* Current)noexcept override;
	BOOL TryLock(UINT Core, Task* Current, AccessMode)noexcept override;
	VOID Unlock(Task* Current)noexcept override;
	VOID Unlock(Task* Current, AccessMode)noexcept override;
};

}