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
class Task;


//========
// Signal
//========

class Signal
{
public:
	// Friends
	friend DispatchedQueue;

	// Con-/Destructors
	Signal();
	~Signal();

	// Common
	inline VOID Cancel() { Trigger(Status::Aborted); }
	inline VOID Trigger(BOOL Success) { Trigger(Success? Status::Success: Status::Error); }
	VOID Trigger(Status Status=Status::Success);
	BOOL Wait();
	BOOL Wait(UINT Timeout);
	BOOL Wait(ScopedLock& Lock);
	BOOL Wait(ScopedLock& Lock, UINT Timeout);

private:
	// Common
	BOOL Succeeded(Task* Task);
	BOOL WaitInternal();
	Handle<Task> m_WaitingTask;
};

}