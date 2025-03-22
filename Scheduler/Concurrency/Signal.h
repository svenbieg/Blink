//==========
// Signal.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class DispatchedQueue;
class Scheduler;
class Task;


//========
// Signal
//========

class Signal
{
public:
	// Friends
	friend DispatchedQueue;
	friend Scheduler;

	// Con-/Destructors
	Signal(): m_Waiting(nullptr) {}
	~Signal() {}

	// Common
	inline VOID Cancel() { Trigger(Status::Aborted); }
	VOID Trigger(Status Status=Status::Success);
	VOID Wait();
	VOID Wait(UINT Timeout);
	VOID Wait(ScopedLock& Lock);
	VOID Wait(ScopedLock& Lock, UINT Timeout);

private:
	// Common
	VOID WaitInternal(ScopedLock& Lock);
	Task* m_Waiting;
};

}