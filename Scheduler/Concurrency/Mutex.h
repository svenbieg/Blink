//=========
// Mutex.h
//=========

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
	~Mutex();

	// Common
	virtual VOID Lock();
	virtual VOID Lock(AccessMode);
	virtual BOOL TryLock();
	virtual BOOL TryLock(AccessMode);
	virtual VOID Unlock();
	virtual VOID Unlock(AccessMode);

protected:
	// Common
	BOOL Lock(UINT Core, Task* Current);
	BOOL Lock(UINT Core, Task* Current, AccessMode);
	BOOL Unlock(Task* Current);
	BOOL Unlock(Task* Current, AccessMode);
	BOOL WakeupWaitingTasks();
	Task* m_Owners=nullptr;
	Task* m_Waiting=nullptr;
};

}