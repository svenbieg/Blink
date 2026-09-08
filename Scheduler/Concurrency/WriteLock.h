//=============
// WriteLock.h
//=============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Signal;


//============
// Write-Lock
//============

class WriteLock
{
public:
	// Friends
	friend Signal;

	// Con-/Destructors
	WriteLock(Mutex& Mutex)noexcept;
	WriteLock(WriteLock const&)=delete;
	~WriteLock()noexcept;

	// Common
	VOID Lock()noexcept;
	VOID Release()noexcept;
	BOOL TryLock()noexcept;
	VOID Unlock()noexcept;

private:
	// Common
	BOOL Lock(UINT Core, Task* Current)noexcept;
	VOID Unlock(UINT Core, Task* Current)noexcept;
	Mutex* m_Mutex;
};

}