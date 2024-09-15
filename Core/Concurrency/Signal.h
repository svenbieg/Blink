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
	Signal();

	// Common
	VOID Broadcast();
	BOOL Wait(ScopedLock& Lock, UINT Timeout=0);

private:
	// Common
	Handle<Task> m_WaitingTask;
};

}
