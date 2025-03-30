//=========
// Mutex.h
//=========

#pragma once


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
	Mutex(): m_Owner(nullptr) {}
	~Mutex() {}

	// Common
	virtual VOID Lock();
	virtual VOID Lock(AccessMode);
	virtual BOOL TryLock();
	virtual BOOL TryLock(AccessMode);
	virtual VOID Unlock();
	virtual VOID Unlock(AccessMode);

protected:
	// Common
	Task* m_Owner;

private:
	// Common
	VOID Yield(SpinLock& SchedulerLock);
	VOID Yield(SpinLock& SchedulerLock, AccessMode Access);
};

}