//=========
// Mutex.h
//=========

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

class Task;


//=======
// Mutex
//=======

class Mutex
{
public:
	// Con-/Destructors
	inline Mutex(): m_Owner(nullptr) {}

	// Common
	VOID Lock();
	VOID Lock(BOOL Blocking);
	VOID LockShared();
	BOOL TryLock();
	BOOL TryLock(BOOL Blocking);
	VOID Unlock();
	VOID UnlockShared();

protected:
	// Common
	Task* m_Owner;
};

}
