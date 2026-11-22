//=============
// WriteLock.h
//=============

// Exclusive lock suspending other tasks.

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

class Scheduler;
class Signal;


//===========
// Lock-Mode
//===========

enum class WriteLockMode
{
Owner
};


//============
// Write-Lock
//============

class WriteLock
{
public:
	// Friends
	friend Scheduler;
	friend Signal;

	// Locked
	class Locked {};

	// Con-/Destructors
	WriteLock(Mutex& Mutex);
	WriteLock(Mutex& Mutex, WriteLockMode);
	WriteLock(WriteLock const&)=delete;
	~WriteLock()noexcept;

	// Common
	VOID Lock();
	VOID Release()noexcept;
	VOID SetLocked(BOOL Locked);
	VOID Unlock()noexcept;

private:
	// Common
	BOOL Lock(Task* Current)noexcept;
	VOID Unlock(Task* Current)noexcept;
	BOOL m_Locked;
	Mutex* m_Mutex;
};

}