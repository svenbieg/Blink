//=========
// Mutex.h
//=========

#pragma once


//=======
// Using
//=======

#include "SpinLock.h"
#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class ScopedLock;
class SharedLock;
class Task;


//========
// Access
//========

enum class AccessMode
{
ReadOnly
};

enum class AccessPriority
{
High
};


//=======
// Mutex
//=======

class Mutex
{
public:
	// Friends
	friend ScopedLock;
	friend SharedLock;

	// Con-/Destructors
	Mutex(): m_Owner(nullptr) {}

	// Common
	VOID Lock();
	VOID Lock(AccessMode);
	VOID Lock(AccessPriority);
	BOOL TryLock();
	BOOL TryLock(AccessMode);
	BOOL TryLock(AccessPriority);
	VOID Unlock();
	VOID Unlock(AccessMode);
	VOID Unlock(AccessPriority);

protected:
	// Common
	Task* m_Owner;

private:
	// Common
	VOID Yield(SpinLock& Lock);
	VOID Yield(SpinLock& Lock, AccessMode);
};

}