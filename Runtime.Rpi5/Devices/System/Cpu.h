//=======
// Cpu.h
//=======

#pragma once


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
	static constexpr UINT CACHE_LINE_SIZE=64;

	// Common
	static VOID CleanDataCache()noexcept;
	static inline BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept
		{
		UINT result;
		__asm inline volatile("\
			prfm pstl1keep, [%1]\n\
			ldaxr %w0, [%1]\n\
			cmp %w0, %w2\n\
			bne 1f\n\
			stxr %w0, %w3, [%1]\n\
			b 2f\n\
			1:\n\
			mov %w0, #1\n\
			2:\n": "=&r" (result): "r" (Value), "r" (Compare), "r" (Set): "cc", "memory");
		return result==0;
		}
	static inline VOID DataStoreBarrier()noexcept
		{
		__asm inline volatile("dsb st"::: "memory");
		}
	static inline VOID DataSyncBarrier()noexcept
		{
		__asm inline volatile("dsb sy"::: "memory");
		}
	static inline VOID DisableInterrupts()noexcept
		{
		__asm inline volatile("msr daifset, #2");
		}
	static inline VOID EnableInterrupts()noexcept
		{
		__asm inline volatile("msr daifclr, #2");
		}
	static inline UINT GetId()noexcept
		{
		UINT id;
		__asm inline volatile("\
			mrs %0, MPIDR_EL1\n\
			lsr %0, %0, #8\n\
			and %0, %0, #3": "=r" (id));
		return id;
		}
	static inline VOID InstructionSyncBarrier()noexcept
		{
		__asm inline volatile("isb"::: "memory");
		}
	static inline VOID InvalidateInstructionCache()noexcept
		{
		__asm inline volatile("ic iallu"::: "memory");
		}
	static inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, VOID* Stack)noexcept
		{
		__asm inline volatile("\
			mov x30, %0\n\
			mov x0, %1\n\
			mov sp, %2\n\
			ret":: "r" (TaskProc), "r" (Parameter), "r" (Stack): "x30", "x0");
		}
	static inline VOID SetEvent()noexcept
		{
		__asm inline volatile("sev");
		}
	static inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)noexcept
		{
		__asm inline volatile("stlr %w1, [%0]":: "r" (Address), "r" (Set));
		}
	static VOID SynchronizeDataAndInstructionCache()noexcept;
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