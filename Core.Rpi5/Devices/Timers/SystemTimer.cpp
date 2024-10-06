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
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
return cnt_pct*CLOCK_MHZ/cnt_freq;
}

Handle<SystemTimer> SystemTimer::Open()
{
if(!s_Current)
	s_Current=new SystemTimer();
return s_Current;
}

VOID SystemTimer::Start()
{
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
UINT64 ticks=cnt_freq/100;
__asm volatile("msr CNTP_CVAL_EL0, %0": : "r" (cnt_pct+ticks));
__asm volatile("msr CNTP_CTL_EL0, %0": : "r" (1UL));
}


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
Interrupts::SetHandler(IRQ_SYSTIMER, HandleInterrupt, this);
m_Task=CreateTask(this, &SystemTimer::TaskProc);
}


//================
// Common Private
//================

VOID SystemTimer::HandleInterrupt(VOID* param)
{
s_Current->m_Signal.Broadcast();
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

VOID SystemTimer::TaskProc()
{
while(m_Task)
	{
	TaskLock lock(m_Mutex);
	m_Signal.Wait(lock);
	Tick(this);
	}
}

Handle<SystemTimer> SystemTimer::s_Current;

}}
