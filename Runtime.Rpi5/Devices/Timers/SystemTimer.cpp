//=================
// SystemTimer.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <io.h>
#include "Concurrency/Task.h"
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

constexpr UINT64 FREQ_HZ=54000000;
constexpr UINT64 KHZ=1000;
constexpr UINT64 MHZ=1000000;
constexpr UINT64 PERIOD=FREQ_HZ/100;


//==================
// Con-/Destructors
//==================

SystemTimer::~SystemTimer()
{
Interrupts::SetHandler(IRQ_SYSTIMER, nullptr);
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


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
m_Task=Task::Create(this, &SystemTimer::ServiceTask, "systimer");
Interrupts::SetHandler(IRQ_SYSTIMER, HandleInterrupt, this);
__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
__asm inline volatile("msr CNTP_CTL_EL0, %0":: "r" (1UL));
}


//================
// Common Private
//================

VOID SystemTimer::HandleInterrupt(VOID* param)
{
auto timer=(SystemTimer*)param;
timer->m_Signal.Trigger();
__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
}

VOID SystemTimer::ServiceTask()
{
auto task=Task::Get();
task->Lock();
while(!task->Cancelled)
	{
	m_Signal.Wait();
	Triggered(this);
	}
}

SystemTimer* SystemTimer::s_Current=nullptr;

}}