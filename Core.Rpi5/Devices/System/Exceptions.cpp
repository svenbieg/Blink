//================
// Exceptions.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Cpu.h"
#include "Exceptions.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//============
// Exceptions
//============

LPCSTR ExceptionName[]=
	{
	"SYNC_EXCEPTION",
	"FIQ_EXCEPTION",
	"ERROR_EXCEPTION",
	"INVALID_EXCEPTION"
	};


//===================
// Exception Handler
//===================

extern "C" VOID HandleException(UINT id, VOID* instr)
{
UINT core=Cpu::GetId();
VOID* stack[1024];
VOID* stack_ptr;
__asm volatile("mov %0, sp": "=r" (stack_ptr));
CopyMemory(stack, stack_ptr, 1024*sizeof(SIZE_T));
DebugPrint("CPU%u: %s (@0x%08x)\n", core, ExceptionName[id], (SIZE_T)instr);
}


//========
// Common
//========

VOID InitializeStack(VOID** stack, VOID (*task_proc)(VOID*), VOID* param)
{
SIZE_T stack_ptr=(SIZE_T)(*stack);
auto irq_frame=(IRQ_FRAME*)(stack_ptr-sizeof(IRQ_FRAME));
irq_frame->X0=(SIZE_T)param;
irq_frame->X30=(SIZE_T)task_proc;
*stack=irq_frame;
}

}}
