//==============
// ScopedLock.h
//==============

#pragma once


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Signal;
class SpinLock;


//=============
// Scoped Lock
//=============

class ScopedLock
{
public:
	// Friends
	friend Signal;

	// Con-/Destructors
	virtual ~ScopedLock() {}

	// Common
	virtual VOID Lock()=0;
	virtual BOOL TryLock()=0;
	virtual VOID Unlock()=0;

protected:
	// Con-/Destructors
	ScopedLock() {}

private:
	// Common
	virtual VOID Yield(SpinLock& Lock)=0;
};

}