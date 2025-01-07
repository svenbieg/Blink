//=========
// Clock.h
//=========

#pragma once


//=======
// Using
//=======

#include "Devices/Timers/SystemTimer.h"
#include "TimePoint.h"


//===========
// Namespace
//===========

namespace Timing {


//=======
// Clock
//=======

class Clock: public Object
{
public:
	// Using
	using SystemTimer=Devices::Timers::SystemTimer;

	// Con-/Destructors
	~Clock();
	static Handle<Clock> Get();

	// Common
	Event<Clock> Day;
	static inline UINT GetDayOfMonth() { return s_Now.DayOfMonth; }
	static inline UINT GetDayOfWeek() { return s_Now.DayOfWeek; }
	static inline UINT GetDayOfYear() { return TimePoint::GetDayOfYear(s_Now); }
	static inline UINT GetHour() { return s_Now.Hour; }
	static inline UINT GetMinute() { return s_Now.Minute; }
	static inline UINT GetMonth() { return s_Now.Month; }
	static inline UINT GetSecond() { return s_Now.Second; }
	static inline BOOL GetTime(TIMEPOINT* Time);
	static inline UINT GetYear() { return s_Now.Year; }
	Event<Clock> Hour;
	static inline BOOL IsSet() { return s_Now.Year!=0; }
	Event<Clock> Minute;
	Event<Clock> Month;
	static TIMEPOINT const& Now();
	Event<Clock> Second;
	VOID SetTime(TIMEPOINT const& Time);
	Event<Clock> Tick;
	Event<Clock> TimeSet;
	static BOOL Update(TIMEPOINT* TimePoint);
	Event<Clock> Year;

private:
	// Con-/Destructors
	Clock();

	// Common
	VOID DoTick();
	VOID OnDay();
	VOID OnHour();
	VOID OnMinute();
	VOID OnMonth();
	VOID OnSecond();
	VOID OnSystemTimerTick();
	VOID OnTick();
	UINT m_Ticks;
	Handle<SystemTimer> m_Timer;
	static TIMEPOINT s_Before;
	static Clock* s_Current;
	static TIMEPOINT s_Now;
	static UINT64 s_Offset;
};

}