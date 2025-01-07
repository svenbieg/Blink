//=========
// Timer.h
//=========

#pragma once


//=======
// Using
//=======

#include "Clock.h"


//===========
// Namespace
//===========

namespace Timing {


//=======
// Timer
//=======

class Timer: public Object
{
public:
	// Con-/Destructors
	~Timer();
	static inline Handle<Timer> Create() { return new Timer(); }

	// Common
	BOOL IsStarted()const { return m_Interval!=0; }
	VOID Reset();
	VOID StartOnce(UINT MilliSeconds);
	VOID StartPeriodic(UINT MilliSeconds);
	VOID Stop();
	Event<Timer> Triggered;

private:
	// Con-/Destructors
	Timer();

	// Common
	VOID DoTrigger();
	VOID OnClockTick();
	INT m_Interval;
	SIZE_T m_NextTime;
	Handle<Clock> m_Clock;
};

}