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
	WriteLock(WriteLock const&)=delete;
	WriteLock(Mutex& Mutex);
	~WriteLock();

	// Common
	VOID Lock()override;
	VOID Release();
	BOOL TryLock()override;
	VOID Unlock()override;

private:
	// Common
	VOID Lock(UINT Core, Task* Current)override;
	VOID Unlock(UINT Core, Task* Current)override;
	Mutex* m_Mutex;
};

}