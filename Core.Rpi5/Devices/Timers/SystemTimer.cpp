//=================
// SystemTimer.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "SystemTimer.h"

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

constexpr UINT64 CLOCK_MHZ=1000000;


//========
// Common
//========

UINT64 SystemTimer::Microseconds64()
{
UINT64 cntpct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cntpct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
return cntpct*CLOCK_MHZ/cnt_freq;
}

Handle<SystemTimer> SystemTimer::Open()
{
if(!s_Current)
	s_Current=new SystemTimer();
return s_Current;
}


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
Interrupts::SetHandler(IRQ_SYSTIMER, HandleInterrupt, this);
s_Task=new Concurrency::Details::TaskTyped(TaskProc);
Scheduler::AddTask(s_Task);
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
UINT64 ticks=cnt_freq/100;
__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_pct+ticks));
__asm volatile("msr CNTP_CTL_EL0, %0": : "r" (1UL));
}


//================
// Common Private
//================

VOID SystemTimer::HandleInterrupt(VOID* param)
{
Scheduler::Schedule();
s_Signal.Broadcast();
//UINT64 cnt_val;
//__asm volatile("mrs %0, CNTP_CVAL_EL0": "=r" (cnt_val));
//__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_val+ticks));
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
UINT64 ticks=cnt_freq/100;
__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_pct+ticks));
}

VOID SystemTimer::TaskProc()
{
while(s_Current)
	{
	TaskLock lock(s_Mutex);
	s_Signal.Wait(lock);
	s_Current->Tick(s_Current);
	}
}

Handle<SystemTimer> SystemTimer::s_Current;
Mutex SystemTimer::s_Mutex;
Signal SystemTimer::s_Signal;
Handle<Task> SystemTimer::s_Task;

}}
