//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include "Settings.h"

namespace Concurrency
{
class Task;
}


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=======
// Using
//=======

extern "C" BOOL cpu_compare_and_set(volatile UINT* value_ptr, UINT compare, UINT set);
extern "C" VOID cpu_context_set(void (*task_proc)(void*), void* param, void* stack);
extern "C" INT cpu_power_on(UINT64 core);


//==========
// Settings
//==========

constexpr UINT CACHE_LINE_SIZE=64;
constexpr BIT_FIELD CPU_ID={ 0x3, 8 };


//=============
// Cpu-Context
//=============

typedef struct
{
VOID* Instruction;
VOID* Parameter;
VOID* Stack;
}CPU_CONTEXT;


//=====
// Cpu
//=====

class Cpu
{
public:
	// Using
	using Task=Concurrency::Task;

	// Common
	static VOID CleanDataCache();
	static inline BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)
		{
		return cpu_compare_and_set(Value, Compare, Set);
		}
	static inline void DataMemoryBarrier()
		{
		__asm volatile("dmb sy"::: "memory");
		}
	static inline VOID DataStoreBarrier()
		{
		__asm volatile("dsb st"::: "memory");
		}
	static inline VOID DataSyncBarrier()
		{
		__asm volatile("dsb sy"::: "memory");
		}
	static inline VOID Delay(UINT Cycles)
		{
		__asm volatile("\
		%=:\n\
		subs %[cycles], %[cycles], #1\n\
		bne %=b\n\
		":: [cycles] "r" (Cycles));
		}
	static inline VOID DisableInterrupts()
		{
		__asm volatile("msr daifset, #2");
		}
	static inline VOID EnableInterrupts()
		{
		__asm volatile("msr daifclr, #2");
		}
	static inline UINT GetId()
		{
		UINT id=0;
		__asm volatile("mrs %[id], mpidr_el1": [id] "=r" (id));
		return Bits::Get(id, CPU_ID);
		}
	static inline VOID InstructionSyncBarrier()
		{
		__asm volatile("isb"::: "memory");
		}
	static inline VOID InvalidateInstructionCache()
		{
		__asm volatile("ic iallu"::: "memory");
		}
	static inline INT PowerOn(UINT Core) { return cpu_power_on(Core); }
	static inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, VOID* Stack)
		{
		cpu_context_set(TaskProc, Parameter, Stack);
		}
	static inline VOID SetEvent()
		{
		__asm volatile("sev");
		}
	static inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)
		{
		__asm volatile("stlr %w1, [%0]":: "r" (Address), "r" (Set));
		}
	static VOID SwitchTask(UINT Core, Task* Current, Task* Next);
	static VOID SynchronizeDataAndInstructionCache();
	static inline VOID WaitForEvent()
		{
		__asm volatile("wfe");
		}
	static inline VOID WaitForInterrupt()
		{
		__asm volatile("wfi");
		}
};

}}
