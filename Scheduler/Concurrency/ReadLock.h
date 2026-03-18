//============
// ReadLock.h
//============

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


//===========
// Read-Lock
//===========

class ReadLock: public ScopedLock
{
public:
	// Con-/Destructors
	ReadLock(ReadLock const&)=delete;
	ReadLock(Mutex& Mutex)noexcept;
	~ReadLock()noexcept;

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