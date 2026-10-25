//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//======================
// Forward-Declarations
//======================

class Interrupts;


//=====
// Cpu
//=====

class Cpu
{
public:
	// Friends
	friend Interrupts;

	// Settings
	static const UINT CACHE_LINE_SIZE=64;
	static const UINT CPU_COUNT=4;

	// Common
	static __force_inline VOID Breakpoint()
		{
		__asm inline volatile("brk #0");
		}
	static VOID CleanDataCache(SIZE_T Address, SIZE_T Size)noexcept;
	static BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept;
	static __force_inline UINT CountLeadingZeros(SIZE_T Value)
		{
		__asm inline volatile("clz %0, %0": "+r" (Value));
		return (UINT)Value;
		}
	static __force_inline UINT CountTrailingZeros(UINT Value)
		{
		__asm inline volatile("\
		rbit %0, %0\n\
		clz %0, %0": "+r" (Value));
		return Value;
		}
	static __force_inline VOID DataMemoryBarrier()noexcept
		{
		__asm inline volatile("dmb ish"::: "memory");
		}
	static __force_inline VOID DataSyncBarrier()noexcept
		{
		__asm inline volatile("dsb sy"::: "memory");
		}
	static VOID Delay(UINT MicroSeconds);
	static __force_inline UINT GetId()noexcept
		{
		UINT id;
		__asm inline volatile("\
			mrs %0, MPIDR_EL1\n\
			lsr %0, %0, #8\n\
			and %0, %0, #3": "=r" (id));
		return id;
		}
	static __force_inline VOID InstructionSyncBarrier()noexcept
		{
		__asm inline volatile("isb"::: "memory");
		}
	static __force_inline UINT InterlockedDecrement(volatile UINT* Value)noexcept
		{
		UINT value;
		__asm inline volatile("\
			mov %w0, #-1\n\
			ldaddal %w0, %w0, [%1]\n\
			": "=&r" (value): "r" (Value));
		return value;
		}
	static __force_inline UINT InterlockedIncrement(volatile UINT* Value)noexcept
		{
		UINT value;
		__asm inline volatile("\
			mov %w0, #1\n\
			ldaddal %w0, %w0, [%1]\n\
			": "=&r" (value): "r" (Value));
		return value;
		}
	static VOID InvalidateDataCache(SIZE_T Address, SIZE_T Size)noexcept;
	static VOID PowerOn(UINT Core);
	static __force_inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, SIZE_T StackPointer)noexcept
		{
		__asm inline volatile("\
			mov x30, %0\n\
			mov x0, %1\n\
			mov sp, %2\n\
			ret":: "r" (TaskProc), "r" (Parameter), "r" (StackPointer): "x30", "x0");
		}
	static __force_inline VOID SetEvent()noexcept
		{
		__asm inline volatile("sev");
		}
	static __force_inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)noexcept
		{
		__asm inline volatile("\
			stlr %w1, [%0]\n\
			dmb ish":: "r" (Address), "r" (Set));
		}
	static __force_inline VOID WaitForEvent()noexcept
		{
		__asm inline volatile("wfe");
		}
	static __force_inline VOID WaitForInterrupt()noexcept
		{
		__asm inline volatile("wfi");
		}
	static VOID WakeupSecondary();

private:
	// Common
	static __force_inline VOID DisableInterrupts()noexcept
		{
		__asm inline volatile("msr daifset, #2");
		}
	static __force_inline VOID EnableInterrupts()noexcept
		{
		__asm inline volatile("msr daifclr, #2");
		}
};

}}