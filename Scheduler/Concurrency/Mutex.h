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
	~Mutex()noexcept;

	// Common
	virtual VOID Lock()noexcept;
	virtual VOID Lock(AccessMode)noexcept;
	virtual BOOL TryLock()noexcept;
	virtual BOOL TryLock(AccessMode)noexcept;
	virtual VOID Unlock()noexcept;
	virtual VOID Unlock(AccessMode)noexcept;

protected:
	// Common
	BOOL Lock(UINT Core, Task* Current)noexcept;
	BOOL Lock(UINT Core, Task* Current, AccessMode)noexcept;
	INT Unlock(Task* Current)noexcept;
	INT Unlock(Task* Current, AccessMode)noexcept;
	UINT WakeupWaitingTasks()noexcept;
	Task* m_Owner=nullptr;
	Task* m_Waiting=nullptr;
};

}