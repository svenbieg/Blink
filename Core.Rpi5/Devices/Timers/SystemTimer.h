//===============
// SystemTimer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"


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
	static inline UINT GetTickCount() { return (UINT)(Microseconds64()/1000); }
	static UINT64 GetTickCount64() { return Microseconds64()/1000; }
	static inline UINT Microseconds() { return (UINT)Microseconds64(); }
	static UINT64 Microseconds64();
	static Handle<SystemTimer> Open();
	Event<SystemTimer> Tick; // ~10ms (100Hz)

private:
	// Con-/Destructors
	SystemTimer();

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID TaskProc();
	Concurrency::Mutex m_Mutex;
	Concurrency::Signal m_Signal;
	Handle<Concurrency::Task> m_Task;
	static SystemTimer* s_Current;
};

}}
