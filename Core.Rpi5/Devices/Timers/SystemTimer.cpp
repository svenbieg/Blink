//=================
// SystemTimer.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "SystemTimer.h"

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
auto timer=(SystemTimer*)param;
timer->Tick(timer);
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
UINT64 ticks=cnt_freq/100;
//UINT64 cnt_val;
//__asm volatile("mrs %0, CNTP_CVAL_EL0": "=r" (cnt_val));
//__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_val+ticks));
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_pct+ticks));
}

Handle<SystemTimer> SystemTimer::s_Current;

}}
