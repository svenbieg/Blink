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
	inline BOOL Wait(UINT Timeout=0) { return Wait(nullptr, Timeout); }
	inline BOOL Wait(ScopedLock& Lock, UINT Timeout=0) { return Wait(&Lock, Timeout); }

private:
	// Common
	BOOL Wait(ScopedLock* Lock, UINT Timeout);
	Handle<Task> m_WaitingTask;
};

}
