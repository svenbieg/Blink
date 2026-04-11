//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

#include "Devices/System/Sio.h"
#include <base.h>


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
	static const UINT CPU_COUNT=2;

	// Common
	static inline void Breakpoint()noexcept
		{
		__asm inline volatile("bkpt #0");
		}
	static inline BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept
		{
		UINT read;
		UINT result=1;
		__asm inline volatile("\
			ldaexb %4, [%1]\n\
			cmp %4, %2\n\
			bne %=f\n\
			strex %0, %3, [%1]\n\
			%=:\n\
			": "=&r" (result): "r" (Value), "r" (Compare), "r" (Set), "r" (read));
		return result==0;
		}
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
	static inline VOID Delay(UINT Cycles)noexcept
		{
		__asm inline volatile("\
		%=:\n\
		subs %[cycles], %[cycles], #1\n\
		bne %=b\n\
		":: [cycles] "r" (Cycles));
		}
	static inline VOID DisableInterrupts()noexcept
		{
		__asm inline volatile("cpsid i"::: "memory");
		}
	static inline VOID EnableInterrupts()noexcept
		{
		__asm inline volatile("cpsie i"::: "memory");
		}
	static inline UINT GetId()noexcept
		{
		auto sio=(SIO_REGS*)SIO_BASE;
		return sio->CPUID;
		}
	static inline VOID InstructionSyncBarrier()noexcept
		{
		__asm inline volatile("isb");
		}
	static inline UINT InterlockedDecrement(volatile UINT* Value)noexcept
		{
		UINT value;
		UINT result;
		__asm inline volatile("\
			%=:\n\
			ldaexb %0, [%1]\n\
			sub %0, %0, #1\n\
			strexb %2, %0, [%1]\n\
			cmp %2, #0\n\
			bne %=b\n\
			": "=&r" (value): "r" (Value), "r" (result));
		return value;
		}
	static inline UINT InterlockedIncrement(volatile UINT* Value)noexcept
		{
		UINT value;
		UINT result;
		__asm inline volatile("\
			%=:\n\
			ldaexb %0, [%1]\n\
			add %0, %0, #1\n\
			strexb %2, %0, [%1]\n\
			cmp %2, #0\n\
			bne %=b\n\
			": "=&r" (value): "r" (Value), "r" (result));
		return value;
		}
	static inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, SIZE_T StackPointer)noexcept
		{
		__asm inline volatile("\
			mov lr, %0\n\
			mov r0, %1\n\
			mov sp, %2\n\
			bx lr":: "r" (TaskProc), "r" (Parameter), "r" (StackPointer): "r0");
		}
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