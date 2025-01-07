//===========
// Timer.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include "Devices/Timers/SystemTimer.h"
#include "Timer.h"

using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Timing {


//==================
// Con-/Destructors
//==================

Timer::~Timer()
{
Stop();
}


//========
// Common
//========

VOID Timer::Reset()
{
INT time=m_Interval>0? m_Interval: -m_Interval;
m_NextTime=SystemTimer::GetTickCount()+time;
}

VOID Timer::StartOnce(UINT ms)
{
assert(ms>=100);
assert(ms%100==0);
if(m_Interval!=0)
	Stop();
m_Interval=ms;
m_NextTime=SystemTimer::GetTickCount()+ms;
m_Clock=Clock::Get();
m_Clock->Tick.Add(this, &Timer::OnClockTick);
}

VOID Timer::StartPeriodic(UINT ms)
{
assert(ms>=100);
assert(ms%100==0);
if(m_Interval!=0)
	Stop();
m_Interval=-(INT)ms;
m_NextTime=SystemTimer::GetTickCount()+ms;
m_Clock=Clock::Get();
m_Clock->Tick.Add(this, &Timer::OnClockTick);
}

VOID Timer::Stop()
{
if(!m_Clock)
	return;
m_Interval=0;
m_NextTime=0;
m_Clock->Tick.Remove(this);
m_Clock=nullptr;
}


//==========================
// Con-/Destructors Private
//==========================

Timer::Timer():
m_Interval(0),
m_NextTime(0)
{}


//================
// Common Private
//================

VOID Timer::DoTrigger()
{
Triggered(this);
}

VOID Timer::OnClockTick()
{
SIZE_T now=SystemTimer::GetTickCount();
if(m_NextTime>now)
	return;
Triggered(this);
if(m_Interval>0)
	{
	Stop();
	return;
	}
UINT time=m_Interval>0? m_Interval: -m_Interval;
m_NextTime=now+time;
}

}