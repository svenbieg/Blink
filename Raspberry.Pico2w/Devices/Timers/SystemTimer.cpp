//=================
// SystemTimer.cpp
//=================

#include "SystemTimer.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
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


//========
// Common
//========

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


//================
// Common Private
//================

VOID SystemTimer::Begin()
{
s_ServiceTask=ServiceTask::Create(ServiceTask, "systimer", 1024);
s_ServiceTask->Then(nullptr, []()
	{
	Interrupts::SetHandler(Irq::Timer0, nullptr);
	});
}

VOID SystemTimer::HandleInterrupt()
{
SpinLock lock(s_CriticalSection);
auto timer=(TIMER_REGS*)TIMER0_BASE;
IoHelper::Set(timer->INTR, 1);
s_Signal.Trigger();
}

VOID SystemTimer::ServiceTask()
{
Clocks::Initialize();
Interrupts::SetHandler(Irq::Timer0, HandleInterrupt);
auto timer=(TIMER_REGS*)TIMER0_BASE;
IoHelper::Set(timer->INTR, 1);
IoHelper::Set(timer->INTE, 1);
SpinLock lock(s_CriticalSection);
UINT time=timer->TIMERAWL;
timer->ALARM[0]=time+10*TICKS_MS;
auto task=Task::Get();
while(!task->Cancelled)
	{
	s_Signal.Wait(lock);
	Scheduler::Schedule();
	time=timer->TIMERAWL;
	timer->ALARM[0]=time+10*TICKS_MS;
	}
}

CriticalSection SystemTimer::s_CriticalSection;
Handle<Task> SystemTimer::s_ServiceTask;
Signal SystemTimer::s_Signal;

}}