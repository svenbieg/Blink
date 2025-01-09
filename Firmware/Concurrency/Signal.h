//==========
// Signal.h
//==========

#pragma once


//=======
// Using
//=======

#include "ScopedLock.h"


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
	inline VOID Cancel() { Trigger(true); }
	VOID Trigger(BOOL Cancel=false);
	BOOL Wait();
	BOOL Wait(UINT Timeout);
	BOOL Wait(ScopedLock& Lock);
	BOOL Wait(ScopedLock& Lock, UINT Timeout);

private:
	// Common
	BOOL WaitInternal();
	Handle<Task> m_WaitingTask;
};

}
