//==========
// Signal.h
//==========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#signal

#pragma once


//=======
// Using
//=======

#include "Status.h"
#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class DispatchedQueue;
class Scheduler;
class ScopedLock;
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
	Signal()=default;
	Signal(Signal const&)=delete;
	~Signal()=default;

	// Common
	inline VOID Cancel() { Trigger(Status::Aborted); }
	VOID Trigger(Status Status=Status::Success);
	VOID Wait(UINT Timeout=0);
	VOID Wait(ScopedLock& Lock, UINT Timeout=0);

private:
	// Common
	VOID WaitInternal(ScopedLock& Lock);
	Task* m_Waiting=nullptr;
};

}