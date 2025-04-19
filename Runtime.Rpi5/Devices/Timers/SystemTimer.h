//===============
// SystemTimer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
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
	static inline UINT Microseconds() { return (UINT)Microseconds64(); }
	static UINT64 Microseconds64();
	Event<SystemTimer> Triggered; // 10ms (100Hz)

private:
	// Con-/Destructors
	SystemTimer();

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID ServiceTask();
	Concurrency::Signal m_Signal;
	Handle<Concurrency::Task> m_Task;
	static SystemTimer* s_Current;
};

}}