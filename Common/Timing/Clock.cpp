//===========
// Clock.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/MainTask.h"
#include "Devices/Timers/SystemTimer.h"
#include "Clock.h"

using namespace Concurrency;
using namespace Culture;


//===========
// Namespace
//===========

namespace Timing {


//==================
// Con-/Destructors
//==================

Clock::~Clock()
{
SystemTimer::Open()->Tick.Remove(this);
}


//========
// Common
//========

Handle<Clock> Clock::Get()
{
if(!s_Current)
	s_Current=new Clock();
return s_Current;
}

BOOL Clock::GetTime(TIMEPOINT* time)
{
if(s_Now.Year==0)
	{
	UINT64 ticks=SystemTimer::GetTickCount();
	MemoryHelper::Copy(time, &ticks, sizeof(UINT64));
	return false;
	}
MemoryHelper::Copy(time, &s_Now, sizeof(TIMEPOINT));
return true;
}

TIMEPOINT const& Clock::Now()
{
if(s_Now.Year==0)
	GetTime(&s_Now);
return s_Now;
}

BOOL Clock::Update(TIMEPOINT* tp)
{
if(tp->Year)
	return false;
if(s_Offset==0)
	return false;
UINT64 ticks=0;
MemoryHelper::Copy(&ticks, tp, sizeof(UINT64));
UINT sec=ticks/1000;
TimePoint::FromSeconds(tp, s_Offset+sec);
return true;
}


//==========================
// Con-/Destructors Private
//==========================

Clock::Clock():
m_Ticks(0)
{
Day.Add(this, &Clock::OnDay);
Hour.Add(this, &Clock::OnHour);
Minute.Add(this, &Clock::OnMinute);
Month.Add(this, &Clock::OnMonth);
Second.Add(this, &Clock::OnSecond);
auto timer=SystemTimer::Open();
timer->Tick.Add(this, &Clock::OnSystemTimerTick);
}


//================
// Common Private
//================

VOID Clock::DoTick()
{
Tick(this);
}

VOID Clock::OnDay()
{
if(s_Before.Month!=s_Now.Month)
	Month(this);
}

VOID Clock::OnHour()
{
if(s_Before.DayOfMonth!=s_Now.DayOfMonth)
	Day(this);
}

VOID Clock::OnMinute()
{
if(s_Before.Hour!=s_Now.Hour)
	Hour(this);
}

VOID Clock::OnMonth()
{
if(s_Before.Year!=s_Now.Year)
	Year(this);
}

VOID Clock::OnSecond()
{
if(s_Before.Minute!=s_Now.Minute)
	Minute(this);
}

VOID Clock::OnSystemTimerTick()
{
if(++m_Ticks%10)
	return;
MainTask::Dispatch(this, &Clock::DoTick);
}

VOID Clock::OnTick()
{
if(m_Ticks%100)
	return;
if(s_Offset)
	{
	s_Before=s_Now;
	UINT64 ticks=SystemTimer::GetTickCount();
	UINT64 sec=s_Offset+ticks/1000;
	TimePoint::FromSeconds(&s_Now, sec);
	}
Second(this);
}

TIMEPOINT Clock::s_Before={ 0 };
Handle<Clock> Clock::s_Current;
TIMEPOINT Clock::s_Now={ 0 };
UINT64 Clock::s_Offset=0;

}