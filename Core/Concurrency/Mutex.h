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
	Mutex(): m_Owner(nullptr) {}

	// Common
	VOID Lock();
	VOID LockBlocking();
	VOID LockShared();
	BOOL TryLock();
	BOOL TryLockBlocking();
	BOOL TryLockShared();
	VOID Unlock();
	VOID UnlockBlocking();
	VOID UnlockShared();

protected:
	// Common
	Task* m_Owner;
};

}
