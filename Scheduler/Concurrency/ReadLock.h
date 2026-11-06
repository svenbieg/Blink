//============
// ReadLock.h
//============

// Read-only lock allowing multiple tasks to read simultaniously.

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
	ReadLock(Mutex& Mutex);
	ReadLock(ReadLock const&)=delete;
	~ReadLock()noexcept;

	// Common
	VOID Lock();
	VOID Release()noexcept;
	VOID Unlock()noexcept;

private:
	// Common
	BOOL m_Locked;
	Mutex* m_Mutex;
};

}