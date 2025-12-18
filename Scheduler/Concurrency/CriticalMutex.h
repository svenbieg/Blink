//=================
// CriticalMutex.h
//=================

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#dynamic-prioritization


//=======
// Using
//=======

#pragma once

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
	~CriticalMutex()=default;

	// Common
	VOID Lock()override;
	VOID Lock(AccessMode)override;
	BOOL TryLock()override;
	BOOL TryLock(AccessMode)override;
	VOID Unlock()override;
	VOID Unlock(AccessMode)override;

private:
	// Common
	VOID AddWaitingTask(Task* Task);
	VOID AddWaitingTask(Task* Task, AccessMode);
};

}