//============
// System.cpp
//============

#include "System.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include <base.h>


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Resets
//========

typedef struct
{
RW32 RESET;
RW32 WDSEL;
RO32 RESET_DONE;
}RESETS_REGS;


//========
// Common
//========

VOID System::Enable(ResetDevice device, UINT timeout)
{
UINT mask=(UINT)device;
auto resets=(RESETS_REGS*)RESETS_BASE;
if(IoHelper::Read(resets->RESET, mask)==0)
	return;
IoHelper::Clear(resets->RESET, mask);
if(timeout)
	{
	IoHelper::Wait(resets->RESET_DONE, mask, mask, timeout);
	}
else
	{
	IoHelper::Retry(resets->RESET_DONE, mask, mask, 100);
	}
}

[[noreturn]] VOID System::PowerOff()
{
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

[[noreturn]] VOID System::Restart()
{
while(1)
	{
	Cpu::WaitForInterrupt();
	}
}

}}