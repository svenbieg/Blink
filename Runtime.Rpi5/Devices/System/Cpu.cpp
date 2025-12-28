//=========
// Cpu.cpp
//=========

#include "Cpu.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
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
constexpr UINT64 PSCI_CPU_ON=0xC4000003;
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

}}