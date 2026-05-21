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
	static inline UINT GetTickCount() { return (UINT)(Microseconds64()/1000); }
	static inline UINT64 GetTickCount64() { return Microseconds64()/1000; }
	static UINT Microseconds();
	static UINT64 Microseconds64();

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