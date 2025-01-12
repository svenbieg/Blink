//=================
// SystemTimer.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Concurrency/TaskLock.h"
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


//==================
// Con-/Destructors
//==================

SystemTimer::~SystemTimer()
{
m_Task->Cancel();
m_Signal.Trigger();
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

UINT64 SystemTimer::Microseconds64()
{
UINT64 cnt_pct;
__asm volatile("mrs %0, CNTPCT_EL0": "=r" (cnt_pct));
UINT64 cnt_freq;
__asm volatile("mrs %0, CNTFRQ_EL0": "=r" (cnt_freq));
return cnt_pct*CLOCK_MHZ/cnt_freq;
}


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
m_Task=Task::Create(this, &SystemTimer::TaskProc, "systimer");
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
s_Current->m_Signal.Trigger();
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
TaskLock lock(m_Mutex);
auto task=Task::Get();
while(!task->Cancelled)
	{
	m_Signal.Wait(lock);
	Triggered(this);
	}
Interrupts::SetHandler(IRQ_SYSTIMER, nullptr, nullptr);
}

SystemTimer* SystemTimer::s_Current=nullptr;

}}
