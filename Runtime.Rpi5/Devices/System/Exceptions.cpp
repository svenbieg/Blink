//================
// Exceptions.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <irq.h>
#include "Cpu.h"
#include "System.h"


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
VOID* stack_ptr;
__asm volatile("mov %0, sp": "=r" (stack_ptr));
// Todo
System::Restart();
}

}}