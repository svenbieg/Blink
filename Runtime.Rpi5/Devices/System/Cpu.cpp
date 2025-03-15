//=========
// Cpu.cpp
//=========

#include "pch.h"


//=======
// Using
//=======

#include <config.h>
#include <irq.h>
#include <task.h>
#include "Concurrency/Task.h"
#include "Devices/System/Cpu.h"
#include "Devices/Timers/SystemTimer.h"

using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=======
// Cache
//=======

constexpr UINT L1_DATA_CACHE_SETS=256;
constexpr UINT L1_DATA_CACHE_WAYS=4;
constexpr UINT L1_SETWAY_WAY_SHIFT=30;

constexpr UINT L2_CACHE_SETS=1024;
constexpr UINT L2_CACHE_WAYS=8;
constexpr UINT L2_SETWAY_WAY_SHIFT=29;

constexpr UINT L3_CACHE_SETS=2048;
constexpr UINT L3_CACHE_WAYS=16;
constexpr UINT L3_SETWAY_WAY_SHIFT=28;

constexpr UINT SETWAY_LEVEL_SHIFT=1;
constexpr UINT SETWAY_SET_SHIFT=6;


//====================================
// Power State Coordination Interface
//====================================

constexpr UINT64 PSCI_CPU_ON=0xC4000003;


//========
// Common
//========

VOID Cpu::CleanDataCache()noexcept
{
for(UINT set=0; set<L1_DATA_CACHE_SETS; set++)
	{
	for(UINT way=0; way<L1_DATA_CACHE_WAYS; way++)
		{
		UINT64 set_way_level=(way<<L1_SETWAY_WAY_SHIFT)|(set<<SETWAY_SET_SHIFT)|(0<<SETWAY_LEVEL_SHIFT);
		__asm volatile("dc csw, %0": : "r" (set_way_level): "memory");
		}
	}
for(UINT set=0; set<L2_CACHE_SETS; set++)
	{
	for(UINT way=0; way<L2_CACHE_WAYS; way++)
		{
		UINT64 set_way_level=(way<<L2_SETWAY_WAY_SHIFT)|(set<<SETWAY_SET_SHIFT)|(1<<SETWAY_LEVEL_SHIFT);
		__asm volatile("dc csw, %0": : "r" (set_way_level): "memory");
		}
	}
for(UINT set=0; set<L3_CACHE_SETS; set++)
	{
	for(UINT way=0; way<L3_CACHE_WAYS; way++)
		{
		UINT64 set_way_level=(way<<L3_SETWAY_WAY_SHIFT)|(set<<SETWAY_SET_SHIFT)|(1<<SETWAY_LEVEL_SHIFT);
		__asm volatile("dc csw, %0": : "r" (set_way_level): "memory");
		}
	}
DataSyncBarrier();
}

VOID Cpu::Delay(UINT us)
{
UINT64 end=SystemTimer::Microseconds64()+us;
while(SystemTimer::Microseconds64()<end);
}

VOID Cpu::SynchronizeDataAndInstructionCache()noexcept
{
CleanDataCache();
InvalidateInstructionCache();
DataSyncBarrier();
InstructionSyncBarrier();
}

}}