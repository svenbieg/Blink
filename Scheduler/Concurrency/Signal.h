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
	Signal();
	~Signal();

	// Common
	inline VOID Cancel() { Trigger(Status::Aborted); }
	VOID Trigger(Status Status=Status::Success);
	VOID Wait();
	VOID Wait(UINT Timeout);
	VOID Wait(ScopedLock& Lock);
	VOID Wait(ScopedLock& Lock, UINT Timeout);

private:
	// Common
	VOID RemoveWaitingTask(Task* Task);
	BOOL WaitInternal();
	Handle<Task> m_WaitingTask;
};

}