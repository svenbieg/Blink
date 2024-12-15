//==========
// Signal.h
//==========

#pragma once


//=======
// Using
//=======

#include "Handle.h"
#include "ScopedLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class Task;


//========
// Signal
//========

class Signal
{
public:
	// Con-/Destructors
	Signal() {}

	// Common
	inline VOID Cancel() { Trigger(true); }
	VOID Trigger(BOOL Cancel=false);
	BOOL Wait();
	BOOL Wait(UINT Timeout);
	BOOL Wait(ScopedLock& Lock);
	BOOL Wait(ScopedLock& Lock, UINT Timeout);

private:
	// Common
	Handle<Task> m_WaitingTask;
};

}
