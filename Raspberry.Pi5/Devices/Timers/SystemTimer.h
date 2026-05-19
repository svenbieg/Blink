//===============
// SystemTimer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"
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
	using Signal=Concurrency::Signal;
	using Task=Concurrency::Task;

	// Friends
	friend Object;

	// Con-/Destructors
	~SystemTimer();

	// Con-/Destructors
	static inline Handle<SystemTimer> Create() { return Global::Create(); }

	// Common
	static inline UINT GetTickCount() { return (UINT)GetTickCount64(); }
	static UINT64 GetTickCount64();
	static inline UINT Microseconds() { return (UINT)Microseconds64(); }
	static UINT64 Microseconds64();
	Event<SystemTimer> Tick; // 10ms (100Hz)

private:
	// Con-/Destructors
	SystemTimer();

	// Common
	VOID HandleInterrupt();
	VOID ServiceTask();
	Signal m_Signal;
	Handle<Task> m_Task;
};

}}