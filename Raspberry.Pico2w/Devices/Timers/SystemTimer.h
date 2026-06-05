//===============
// SystemTimer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalSection.h"
#include "Concurrency/Signal.h"
#include "Concurrency/Task.h"


//======================
// Forward-Declarations
//======================

namespace Concurrency
{
class Scheduler;
}


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//==============
// System-Timer
//==============

class SystemTimer
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using Scheduler=Concurrency::Scheduler;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Friends
	friend Object;
	friend Scheduler;

	// Common
	static inline UINT64 GetTickCount() { return Microseconds()/1000; }
	static UINT64 Microseconds();

private:
	// Settings
	static const UINT TICKS_MS=1000;

	// Common
	static VOID Begin();
	static VOID HandleInterrupt();
	static VOID ServiceTask();
	static CriticalSection s_CriticalSection;
	static Signal s_Signal;
	static Handle<Task> s_ServiceTask;
};

}}