//============
// System.cpp
//============

#include "System.h"


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/System/Cpu.h"
#include "Devices/IoHelper.h"
#include "Devices/Peripherals.h"

using namespace Devices::Gpio;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//===============
// Power Manager
//===============

typedef struct
{
RW32 RES0[6];
RW32 RSTC;
RW32 RSTS;
RW32 WDOG;
RW32 RES1;
RW32 PADS[3];
}PM_REGS;

const UINT PM_PASSWD=(0x5A<<24);
const UINT PM_FULL_RESET=0x20;


//=======
// Reset
//=======

typedef struct
{
RW32 SET;
RW32 CLEAR;
RW32 STATUS;
RW32 RES[3];
}INIT_BANK_REGS;

typedef struct
{
INIT_BANK_REGS INIT_BANK[2];
}RESET_REGS;


//========
// Common
//========

VOID System::Enable(ResetDevice device)
{
if(device==ResetDevice::None)
	return;
auto reset=(RESET_REGS*)AXI_RESET_BASE;
UINT bank=(UINT)device/32;
UINT mask=(UINT)device&0x1F;
IoHelper::Write(reset->INIT_BANK[bank].CLEAR, mask);
IoHelper::Wait(reset->INIT_BANK[bank].STATUS, mask, mask);
}

VOID System::PowerOff()
{
const UINT64 PSCI_POWER_OFF=0x84000008;
__asm volatile("\
mov	x0, %0\n\
smc	#0\n\
":: "r" (PSCI_POWER_OFF));
while(1)
	Cpu::WaitForInterrupt();
}

VOID System::Reset()
{
// Todo: Clear Settings
Restart();
}

VOID System::Restart()
{
const UINT64 PSCI_RESET=0x84000009;
__asm volatile("\
mov	x0, %0\n\
smc	#0\n\
":: "r" (PSCI_RESET));
while(1)
	Cpu::WaitForInterrupt();
}

}}