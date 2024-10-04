//===============
// SystemTimer.h
//===============

#pragma once


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
	// Common
	static inline UINT Microseconds() { return (UINT)Microseconds64(); }
	static UINT64 Microseconds64();
	static inline UINT Milliseconds() { return (UINT)(Microseconds64()/1000); }
	static UINT64 Milliseconds64() { return Microseconds64()/1000; }
	static Handle<SystemTimer> Open();
	Event<SystemTimer> Tick; // ~10ms (100Hz)

private:
	// Con-/Destructors
	SystemTimer();

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	static VOID TaskProc();
	static Handle<SystemTimer> s_Current;
	static Concurrency::Mutex s_Mutex;
	static Concurrency::Signal s_Signal;
	static Handle<Concurrency::Task> s_Task;
};

}}
