//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include "Runtime/Configuration.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=====
// Cpu
//=====

class Cpu
{
public:
	// Settings
	static const UINT CACHE_LINE_SIZE=8;
	static const UINT CPU_COUNT=Runtime::CONFIG_CPU_COUNT;

	// Common
	static inline void Breakpoint()noexcept
		{
		__asm inline volatile("bkpt #0");
		}
	static BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept;
	static inline UINT CountLeadingZeros(UINT Value)
		{
		__asm inline volatile("clz %0, %0": "+r" (Value));
		return Value;
		}
	static inline UINT CountTrailingZeros(UINT Value)
		{
		__asm inline volatile("\
		rbit %0, %0\n\
		clz %0, %0": "+r" (Value));
		return Value;
		}
	static inline VOID DataMemoryBarrier()noexcept
		{
		__asm inline volatile("dmb ish"::: "memory");
		}
	static inline VOID DataSyncBarrier()noexcept
		{
		__asm inline volatile("dsb sy"::: "memory");
		}
	static VOID Delay(UINT Cycles)noexcept;
	static inline VOID DisableInterrupts()noexcept
		{
		__asm inline volatile("cpsid i"::: "memory");
		}
	static inline VOID EnableInterrupts()noexcept
		{
		__asm inline volatile("cpsie i"::: "memory");
		}
	static UINT GetId()noexcept;
	static inline VOID InstructionSyncBarrier()noexcept
		{
		__asm inline volatile("isb");
		}
	static UINT InterlockedDecrement(volatile UINT* Value)noexcept;
	static UINT InterlockedIncrement(volatile UINT* Value)noexcept;
	static VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, SIZE_T StackPointer)noexcept;
	static inline VOID SetEvent()noexcept
		{
		__asm inline volatile("sev");
		}
	static inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)noexcept
		{
		__asm inline volatile("stl %1, [%0]":: "r" (Address), "r" (Set));
		}
	static inline VOID WaitForEvent()noexcept
		{
		__asm inline volatile("wfe");
		}
	static inline VOID WaitForInterrupt()noexcept
		{
		__asm inline volatile("wfi");
		}
};

}}