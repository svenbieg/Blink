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


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Read-Lock
//===========

class ReadLock
{
public:
	// Con-/Destructors
	ReadLock(ReadLock const&)=delete;
	ReadLock(Mutex& Mutex)noexcept;
	~ReadLock()noexcept;

	// Common
	VOID Lock()noexcept;
	VOID Release()noexcept;
	BOOL TryLock()noexcept;
	VOID Unlock()noexcept;

private:
	// Common
	Mutex* m_Mutex;
};

}