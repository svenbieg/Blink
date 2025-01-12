//=======
// Cpu.h
//=======

#pragma once


//=======
// Using
//=======

namespace Concurrency
{
class Task;
}


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

	// Using
	using Task=Concurrency::Task;

	// Common
	static VOID CleanDataCache()noexcept;
	static inline BOOL CompareAndSet(volatile UINT* Value, UINT Compare, UINT Set)noexcept
		{
		UINT result;
		__asm volatile("\
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
	static inline VOID DisableInterrupts()noexcept
		{
		__asm volatile("msr daifset, #2");
		}
	static inline VOID EnableInterrupts()noexcept
		{
		__asm volatile("msr daifclr, #2");
		}
	static inline UINT GetId()noexcept
		{
		UINT id;
		__asm volatile("\
			mrs %0, mpidr_el1\n\
			lsr %0, %0, #8\n\
			and %0, %0, #3": "=r" (id));
		return id;
		}
	static ALWAYS_INLINE SIZE_T GetStackPointer()noexcept
		{
		SIZE_T stack;
		__asm volatile("mov %0, sp": "=r" (stack));
		return stack;
		}
	static inline VOID InstructionSyncBarrier()noexcept
		{
		__asm volatile("isb"::: "memory");
		}
	static inline VOID InvalidateInstructionCache()noexcept
		{
		__asm volatile("ic iallu"::: "memory");
		}
	static inline VOID SetContext(VOID (*TaskProc)(VOID*), VOID* Parameter, VOID* Stack)noexcept
		{
		__asm volatile("\
			mov x30, %0\n\
			mov x0, %1\n\
			mov sp, %2\n\
			ret":: "r" (TaskProc), "r" (Parameter), "r" (Stack): "x30", "x0");
		}
	static inline VOID SetEvent()noexcept
		{
		__asm volatile("sev");
		}
	static inline VOID StoreAndRelease(volatile UINT* Address, UINT Set)noexcept
		{
		__asm volatile("stlr %w1, [%0]":: "r" (Address), "r" (Set));
		}
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