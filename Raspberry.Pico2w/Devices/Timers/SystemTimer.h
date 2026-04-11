//===============
// SystemTimer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Timers/Clocks.h"
#include "Event.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//==============
// System-Timer
//==============

class SystemTimer: public Object
{
public:
	// Con-/Destructors
	~SystemTimer();

	// Common
	static Handle<SystemTimer> Get();
	static inline UINT GetTickCount() { return (UINT)(Microseconds64()/1000); }
	static inline UINT64 GetTickCount64() { return Microseconds64()/1000; }
	static UINT Microseconds();
	static UINT64 Microseconds64();
	Event<SystemTimer> Triggered; // ~10ms (100Hz)

private:
	// Settings
	static const UINT TICKS_MS=1000;

	// Con-/Destructors
	SystemTimer();

	// Common
	VOID OnInterrupt();
	VOID TaskProc();
	Concurrency::CriticalSection m_CriticalSection;
	Concurrency::Signal m_Signal;
	Handle<Concurrency::Task> m_Task;
	static SystemTimer* s_Current;
};

}}