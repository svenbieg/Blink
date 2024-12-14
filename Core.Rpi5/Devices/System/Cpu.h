//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include "Exceptions.h"
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
extern "C" VOID cpu_context_restore(EXC_FRAME* context);
extern "C" VOID cpu_context_save(EXC_FRAME* context);
extern "C" VOID cpu_context_set(void (*task_proc)(void*), void* param, void* stack);
extern "C" UINT cpu_id();
extern "C" INT cpu_power_on(UINT64 core);


//==========
// Settings
//==========

constexpr UINT CACHE_LINE_SIZE=64;


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
	static VOID CleanDataCache()noexcept;
	static inline BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept { return cpu_compare_and_set(Value, Compare, Set); }
	static inline VOID DataMemoryBarrier()noexcept
		{
		__asm volatile("dmb sy"::: "memory");
		}
	static inline VOID DataStoreBarrier()noexcept
		{
		__asm volatile("dsb st"::: "memory");
		}
	static inline VOID DataSyncBarrier()noexcept
		{
		__asm volatile("dsb sy"::: "memory");
		}
	//static inline VOID Delay(UINT Cycles)
	//	{
	//	__asm volatile("\
	//	%=:\n\
	//	subs %[cycles], %[cycles], #1\n\
	//	bne %=b\n\
	//	":: [cycles] "r" (Cycles));
	//	}
	static inline VOID DisableInterrupts()noexcept
		{
		__asm volatile("msr daifset, #2");
		}
	static inline VOID EnableInterrupts()noexcept
		{
		__asm volatile("msr daifclr, #2");
		}
	static inline UINT GetId()noexcept { return cpu_id(); }
	static inline VOID InstructionSyncBarrier()noexcept
		{
		__asm volatile("isb"::: "memory");
		}
	static inline VOID InvalidateInstructionCache()noexcept
		{
		__asm volatile("ic iallu"::: "memory");
		}
	static inline INT PowerOn(UINT Core)noexcept { return cpu_power_on(Core); }
	static inline VOID RestoreContext(EXC_FRAME* Context)noexcept { cpu_context_restore(Context); }
	static inline VOID SaveContext(EXC_FRAME* Context)noexcept { cpu_context_save(Context); }
	static inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, VOID* Stack)noexcept { cpu_context_set(TaskProc, Parameter, Stack); }
	static inline VOID SetEvent()noexcept
		{
		__asm volatile("sev");
		}
	static inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)noexcept
		{
		__asm volatile("stlr %w1, [%0]":: "r" (Address), "r" (Set));
		}
	static VOID SwitchTask(UINT Core, Task* Current, Task* Next)noexcept;
	static VOID SynchronizeDataAndInstructionCache()noexcept;
	static inline VOID WaitForEvent()noexcept
		{
		__asm volatile("wfe");
		}
	static inline VOID WaitForInterrupt()noexcept
		{
		__asm volatile("wfi");
		}
};

}}
