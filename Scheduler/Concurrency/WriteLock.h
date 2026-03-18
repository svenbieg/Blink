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
#include "Concurrency/ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//============
// Write-Lock
//============

class WriteLock: public ScopedLock
{
public:
	// Con-/Destructors
	WriteLock(Mutex& Mutex)noexcept;
	WriteLock(WriteLock const&)=delete;
	~WriteLock()noexcept;

	// Common
	VOID Lock()noexcept override;
	VOID Release()noexcept;
	BOOL TryLock()noexcept override;
	VOID Unlock()noexcept override;

private:
	// Common
	VOID Lock(UINT Core, Task* Current)noexcept override;
	VOID Unlock(UINT Core, Task* Current)noexcept override;
	Mutex* m_Mutex;
};

}