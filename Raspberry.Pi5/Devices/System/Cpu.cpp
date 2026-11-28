//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/MailBox.h"
#include "Devices/Timers/SystemTimer.h"

using namespace Devices::Timers;

extern "C" VOID init();


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID Cpu::CleanDataCache(SIZE_T addr, SIZE_T size)noexcept
{
SIZE_T start=TypeHelper::AlignDown(addr, CACHE_LINE_SIZE);
SIZE_T end=TypeHelper::AlignUp(addr+size, CACHE_LINE_SIZE);
for(SIZE_T pos=start; pos<end; pos+=CACHE_LINE_SIZE)
	__asm volatile("dc cvac, %0":: "r" (pos) : "memory");
DataSyncBarrier();
}

BOOL Cpu::CompareAndSet(volatile UINT* value_ptr, UINT cmp, UINT set)noexcept
{
UINT read;
UINT result=1;
__asm inline volatile("\
prfm pstl1keep, [%[addr]]\n\
ldaxr %w[read], [%[addr]]\n\
cmp %w[read], %w[cmp]\n\
bne failed%=\n\
stlxr %w[res], %w[set], [%[addr]]\n\
b done%=\n\
failed%=:\n\
clrex\n\
done%=:\n\
": [res] "+&r" (result), [read] "=&r" (read): [addr] "r" (value_ptr), [cmp] "r" (cmp), [set] "r" (set): "cc", "memory");
return result==0;
}

VOID Cpu::Delay(UINT us)
{
UINT64 end=SystemTimer::Microseconds64()+us;
while(SystemTimer::Microseconds64()<end);
}

UINT Cpu::GetTemperature()
{
UINT temp[2];
MailBox::GetProperty(PropertyTag::GetTemperature, temp, sizeof(temp));
return temp[1]/1000;
}

VOID Cpu::InvalidateDataCache(SIZE_T addr, SIZE_T size)noexcept
{
SIZE_T start=TypeHelper::AlignDown(addr, CACHE_LINE_SIZE);
SIZE_T end=TypeHelper::AlignUp(addr+size, CACHE_LINE_SIZE);
for(SIZE_T pos=start; pos<end; pos+=CACHE_LINE_SIZE)
	__asm volatile("dc ivac, %0":: "r" (pos) : "memory");
DataSyncBarrier();
}

VOID Cpu::WakeupSecondary()
{
for(UINT core=1; core<CPU_COUNT; core++)
	PowerOn(core);
}


//================
// Common Private
//================

VOID Cpu::PowerOn(UINT core)
{
const UINT64 PSCI_CPU_ON=0xC4000003;
register UINT64 x0 asm("x0")=PSCI_CPU_ON;
register UINT64 x1 asm("x1")=(core<<8);
register UINT64 x2 asm("x2")=(SIZE_T)&init;
register UINT64 x3 asm("x3")=0;
__asm volatile("smc #0": "+&r" (x0): "r" (x1), "r" (x2), "r" (x3));
if(x0!=0)
	throw DeviceNotReadyException();
}

}}