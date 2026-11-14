//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/Timers/SystemTimer.h"

using namespace Devices::Timers;

extern "C" VOID init_secondary();


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
SIZE_T end=addr+size;
while(addr<end)
	{
	__asm volatile("dc cvac, %0":: "r" (addr) : "memory");
	addr+=CACHE_LINE_SIZE;
	}
}

BOOL Cpu::CompareAndSet(volatile UINT* value_ptr, UINT cmp, UINT set)noexcept
{
UINT read=0;
UINT result=1;
__asm inline volatile("\
prfm pstl1keep, [%1]\n\
ldaxr %w4, [%1]\n\
cmp %w4, %w2\n\
bne done%=\n\
stxr %w0, %w3, [%1]\n\
done%=:\n\
": "+&r" (result): "r" (value_ptr), "r" (cmp), "r" (set), "r" (read): "cc", "memory");
return result==0;
}

VOID Cpu::Delay(UINT us)
{
UINT64 end=SystemTimer::Microseconds64()+us;
while(SystemTimer::Microseconds64()<end);
}

VOID Cpu::InvalidateDataCache(SIZE_T addr, SIZE_T size)noexcept
{
SIZE_T end=addr+size;
while(addr<end)
	{
	__asm volatile("dc ivac, %0":: "r" (addr) : "memory");
	addr+=CACHE_LINE_SIZE;
	}
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
register UINT64 x2 asm("x2")=(SIZE_T)&init_secondary;
register UINT64 x3 asm("x3")=0;
__asm volatile("\
smc #0": "+&r" (x0), "+&r" (x1), "+&r" (x2), "+&r" (x3):: "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x30", "memory", "cc");
if(x0!=0)
	throw DeviceNotReadyException();
}

}}