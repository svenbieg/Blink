//============
// System.cpp
//============

#include "System.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/Peripherals.h"


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
auto resets=(RESETS_REGS*)RESETS_BASE;
UINT mask=(UINT)device;
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

VOID System::Disable(ResetDevice device)
{
auto resets=(RESETS_REGS*)RESETS_BASE;
UINT mask=(UINT)device;
IoHelper::Set(resets->RESET, mask);
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