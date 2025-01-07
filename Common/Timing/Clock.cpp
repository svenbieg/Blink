//===========
// Clock.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
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
if(m_Timer)
	{
	m_Timer->Triggered.Remove(this);
	m_Timer=nullptr;
	}
s_Current=nullptr;
}

Handle<Clock> Clock::Get()
{
if(!s_Current)
	s_Current=new Clock();
return s_Current;
}


//========
// Common
//========

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

VOID Clock::SetTime(TIMEPOINT const& tp)
{
UINT64 now=SystemTimer::GetTickCount()/1000;
UINT64 secs=TimePoint::ToSeconds(tp);
s_Offset=secs-now;
s_Before=s_Now;
s_Now=tp;
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
s_Current=this;
Day.Add(this, &Clock::OnDay);
Hour.Add(this, &Clock::OnHour);
Minute.Add(this, &Clock::OnMinute);
Month.Add(this, &Clock::OnMonth);
Second.Add(this, &Clock::OnSecond);
m_Timer=SystemTimer::Get();
m_Timer->Triggered.Add(this, &Clock::OnSystemTimerTick);
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
DispatchedQueue::Append(this, &Clock::DoTick);
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
Clock* Clock::s_Current=nullptr;
TIMEPOINT Clock::s_Now={ 0 };
UINT64 Clock::s_Offset=0;

}