//==============
// ScopedLock.h
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Signal;
class SpinLock;
class Task;


//=============
// Scoped Lock
//=============

class ScopedLock
{
public:
	// Friends
	friend Signal;

	// Con-/Destructors
	virtual ~ScopedLock()=default;

	// Common
	virtual VOID Lock()=0;
	virtual BOOL TryLock()=0;
	virtual VOID Unlock()=0;

protected:
	// Con-/Destructors
	ScopedLock()=default;

private:
	// Common
	virtual VOID Yield(SpinLock& SchedulerLock, UINT Core, Task* Current)=0;
};

}