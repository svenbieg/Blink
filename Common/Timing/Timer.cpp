//===========
// Timer.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

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

Timer::Timer():
m_Interval(0),
m_NextTime(0)
{}

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
if(m_Interval!=0)
	Stop();
if(ms==0)
	return;
m_Interval=ms;
m_NextTime=SystemTimer::GetTickCount()+ms;
auto timer=SystemTimer::Open();
timer->Tick.Add(this, &Timer::OnClockTick);
}

VOID Timer::StartPeriodic(UINT ms)
{
if(m_Interval!=0)
	Stop();
assert(ms>=10);
m_Interval=-(INT)ms;
m_NextTime=SystemTimer::GetTickCount()+ms;
auto timer=SystemTimer::Open();
timer->Tick.Add(this, &Timer::OnClockTick);
}

VOID Timer::Stop()
{
if(m_Interval==0)
	return;
auto timer=SystemTimer::Open();
timer->Tick.Remove(this);
m_Interval=0;
m_NextTime=0;
}


//================
// Common Private
//=================

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
	Stop();
UINT time=m_Interval>0? m_Interval: -m_Interval;
m_NextTime=now+time;
}

}