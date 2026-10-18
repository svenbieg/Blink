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

extern "C" VOID _start();


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

VOID Cpu::PowerOn(UINT core)
{
const UINT64 PSCI_CPU_ON=0xC4000003;
UINT64 mpidr=(core<<8);
INT status=0;
__asm volatile("\
mov x0, %1\n\
mov x1, %2\n\
mov x2, %3\n\
mov x3, #1\n\
mov x4, xzr\n\
smc #0\n\
mov %w0, w0": "=r" (status): "r" (PSCI_CPU_ON), "r" (mpidr), "r" (&_start));
if(status!=0)
	throw DeviceNotReadyException();
}

VOID Cpu::WakeupSecondary()
{
}

}}