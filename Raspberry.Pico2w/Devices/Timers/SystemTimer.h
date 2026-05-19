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
#include "Devices/Timers/Clocks.h"
#include "Event.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//==============
// System-Timer
//==============

class SystemTimer: public Global<SystemTimer>
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<SystemTimer> Create() { return Global::Create(); }
	~SystemTimer();

	// Common
	static inline UINT GetTickCount() { return (UINT)(Microseconds64()/1000); }
	static inline UINT64 GetTickCount64() { return Microseconds64()/1000; }
	static UINT Microseconds();
	static UINT64 Microseconds64();
	Event<SystemTimer> Tick; // ~10ms (100Hz)

private:
	// Settings
	static const UINT TICKS_MS=1000;

	// Con-/Destructors
	SystemTimer();

	// Common
	VOID OnInterrupt();
	VOID TaskProc();
	CriticalSection m_CriticalSection;
	Signal m_Signal;
	Handle<Task> m_Task;
};

}}