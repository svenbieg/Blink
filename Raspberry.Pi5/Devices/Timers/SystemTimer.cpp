//=================
// SystemTimer.cpp
//=================

#include "SystemTimer.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Interrupts.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Timers {


//==========
// Settings
//==========

const UINT64 FREQ_HZ=54000000;
const UINT64 KHZ=1000;
const UINT64 MHZ=1000000;
const UINT64 PERIOD=FREQ_HZ/100;


//========
// Common
//========

UINT64 SystemTimer::GetTickCount64()
{
UINT64 cnt_pct;
__asm inline volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
return cnt_pct*KHZ/FREQ_HZ;
}

UINT64 SystemTimer::Microseconds64()
{
UINT64 cnt_pct;
__asm inline volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
return cnt_pct*MHZ/FREQ_HZ;
}


//================
// Common Private
//================

VOID SystemTimer::HandleInterrupt()
{
__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
s_Signal.Trigger();
}

VOID SystemTimer::ServiceTask()
{
Interrupts::SetHandler(Irq::SystemTimer, HandleInterrupt);
__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
__asm inline volatile("msr CNTP_CTL_EL0, %0":: "r" (1UL));
while(1)
	{
	s_Signal.Wait();
	Scheduler::Schedule();
	}
}

VOID SystemTimer::Start()
{
s_ServiceTask=ServiceTask::Create(ServiceTask, "systimer", 1024);
}

Handle<Task> SystemTimer::s_ServiceTask;
Signal SystemTimer::s_Signal;

}}