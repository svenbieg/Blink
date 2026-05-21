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
	static inline UINT GetTickCount() { return (UINT)GetTickCount64(); }
	static UINT64 GetTickCount64();
	static inline UINT Microseconds() { return (UINT)Microseconds64(); }
	static UINT64 Microseconds64();

private:
	// Common
	static VOID Begin();
	static VOID HandleInterrupt();
	static VOID ServiceTask();
	static CriticalSection s_CriticalSection;
	static Handle<Task> s_ServiceTask;
	static Signal s_Signal;
};

}}