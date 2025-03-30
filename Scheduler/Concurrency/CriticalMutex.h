//=================
// CriticalMutex.h
//=================

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
	CriticalMutex() {}
	~CriticalMutex() {}

	// Common
	VOID Lock()override;
	VOID Lock(AccessMode)override;
	BOOL TryLock()override;
	BOOL TryLock(AccessMode)override;
	VOID Unlock()override;
	VOID Unlock(AccessMode)override;
};

}