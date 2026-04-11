//=================
// SystemTimer.cpp
//=================

#include "SystemTimer.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/System/Interrupts.h"
#include "Devices/Timers/Clocks.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//=======
// Timer
//=======

typedef struct
{
WO32 TIMEHW;
WO32 TIMELW;
RO32 TIMEHR;
RO32 TIMELR;
RW32 ALARM[4];
RW32 ARMED;
RO32 TIMERAWH;
RO32 TIMERAWL;
RW32 DBGPAUSE;
RW32 PAUSE;
RW32 LOCKED;
RW32 SOURCE;
RW32 INTR;
RW32 INTE;
RW32 INTF;
RO32 INTS;
}TIMER_REGS;


//==================
// Con-/Destructors
//==================

SystemTimer::~SystemTimer()
{
m_Task->Cancel();
s_Current=nullptr;
}


//========
// Common
//========

Handle<SystemTimer> SystemTimer::Get()
{
if(!s_Current)
	s_Current=new SystemTimer();
return s_Current;
}

UINT SystemTimer::Microseconds()
{
auto timer=(TIMER_REGS*)TIMER0_BASE;
return timer->TIMERAWL;
}

UINT64 SystemTimer::Microseconds64()
{
auto timer=(TIMER_REGS*)TIMER0_BASE;
UINT hi=0;
UINT lo=0;
do
	{
	hi=timer->TIMERAWH;
	lo=timer->TIMERAWL;
	}
while(timer->TIMERAWH!=hi);
return TypeHelper::MakeLong(lo, hi);
}


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
Clocks::Initialize();
m_Task=ServiceTask::Create(this, &SystemTimer::TaskProc, "systimer");
}


//================
// Common Private
//================

VOID SystemTimer::OnInterrupt()
{
auto timer=(TIMER_REGS*)TIMER0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(timer->INTR, 1);
s_Current->m_Signal.Trigger();
}

VOID SystemTimer::TaskProc()
{
Interrupts::SetHandler(Irq::Timer0, this, &SystemTimer::OnInterrupt);
auto timer=(TIMER_REGS*)TIMER0_BASE;
IoHelper::Set(timer->INTR, 1);
IoHelper::Set(timer->INTE, 1);
SpinLock lock(m_CriticalSection);
UINT time=timer->TIMERAWL;
timer->ALARM[0]=time+10*TICKS_MS;
auto task=Task::Get();
while(!task->Cancelled)
	{
	m_Signal.Wait(lock);
	lock.Unlock();
	Triggered(this);
	lock.Lock();
	time=timer->TIMERAWL;
	timer->ALARM[0]=time+10*TICKS_MS;
	}
Interrupts::SetHandler(Irq::Timer0, nullptr);
}

SystemTimer* SystemTimer::s_Current=nullptr;

}}
