//=========
// Mutex.h
//=========

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#mutex


//=======
// Using
//=======

#pragma once

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class ReadLock;
class SpinLock;
class Task;
class WriteLock;


//=============
// Access-Mode
//=============

enum class AccessMode
{
ReadOnly
};


//=======
// Mutex
//=======

class Mutex
{
public:
	// Friends
	friend ReadLock;
	friend WriteLock;

	// Con-/Destructors
	Mutex()=default;
	Mutex(Mutex const&)=delete;
	~Mutex()=default;

	// Common
	virtual VOID Lock();
	virtual VOID Lock(AccessMode);
	virtual BOOL TryLock();
	virtual BOOL TryLock(AccessMode);
	virtual VOID Unlock();
	virtual VOID Unlock(AccessMode);

protected:
	// Common
	BOOL LockInternal(UINT Core, Task* Current);
	BOOL LockInternal(UINT Core, Task* Current, AccessMode);
	VOID UnlockInternal(UINT Core, Task* Current);
	VOID UnlockInternal(UINT Core, Task* Current, AccessMode);
	Task* m_Owner=nullptr;
	Task* m_Waiting=nullptr;

private:
	// Common
	VOID AddWaitingTask(Task* Task);
	VOID AddWaitingTask(Task* Task, AccessMode);
	VOID Yield(SpinLock& SchedulerLock);
	VOID Yield(SpinLock& SchedulerLock, AccessMode Access);
};

}