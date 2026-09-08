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
	Mutex(): m_Owner(nullptr), m_Waiting(nullptr) {}
	Mutex(Mutex const&)=delete;
	~Mutex()noexcept;

	// Common
	VOID Lock()noexcept;
	VOID Lock(AccessMode)noexcept;
	BOOL TryLock()noexcept;
	BOOL TryLock(AccessMode)noexcept;
	VOID Unlock()noexcept;
	VOID Unlock(AccessMode)noexcept;

protected:
	// Common
	virtual BOOL Lock(UINT Core, Task* Current)noexcept;
	virtual BOOL Lock(UINT Core, Task* Current, AccessMode)noexcept;
	VOID ResumeWaitingTasks()noexcept;
	virtual BOOL TryLock(UINT Core, Task* Current)noexcept;
	virtual BOOL TryLock(UINT Core, Task* Current, AccessMode)noexcept;
	virtual VOID Unlock(Task* Current)noexcept;
	virtual VOID Unlock(Task* Current, AccessMode)noexcept;
	Task* m_Owner;
	Task* m_Waiting;
};

}