//=================
// SystemTimer.cpp
//=================

#include "SystemTimer.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/ServiceTask.h"
#include "Concurrency/SpinLock.h"
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


//==================
// Con-/Destructors
//==================

SystemTimer::~SystemTimer()
{
Interrupts::SetHandler(Irq::SystemTimer, nullptr);
m_Task->Cancel();
}


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


//==========================
// Con-/Destructors Private
//==========================

SystemTimer::SystemTimer()
{
m_Task=ServiceTask::Create(this, &SystemTimer::ServiceTask, "systimer");
}


//================
// Common Private
//================

VOID SystemTimer::HandleInterrupt()
{
SpinLock lock(m_CriticalSection);
m_Signal.Trigger();
}

VOID SystemTimer::ServiceTask()
{
Interrupts::SetHandler(Irq::SystemTimer, this, &SystemTimer::HandleInterrupt);
auto task=Task::Get();
SpinLock lock(m_CriticalSection);
__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
__asm inline volatile("msr CNTP_CTL_EL0, %0":: "r" (1UL));
while(!task->Cancelled)
	{
	m_Signal.Wait(lock);
	lock.Unlock();
	auto handler=DispatchedQueue::Append(this, [this](){ Tick(this); });
	handler->Wait();
	lock.Lock();
	__asm inline volatile("msr CNTP_TVAL_EL0, %0":: "r" (PERIOD));
	}
}

}}