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

class ReadLock;
class Task;
class TaskLock;
class WriteLock;


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
	friend ReadLock;
	friend TaskLock;
	friend WriteLock;

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

private:
	// Common
	VOID Yield(SpinLock& Lock);
	VOID Yield(SpinLock& Lock, AccessMode Access);
	VOID Yield(SpinLock& Lock, AccessPriority Priority);
	Task* m_Owner;
};

}