//============
// System.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <base.h>
#include <io.h>
#include "Devices/Gpio/GpioHelper.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/System.h"

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
rw32_t RES0[6];
rw32_t RSTC;
rw32_t RSTS;
rw32_t WDOG;
rw32_t RES1;
rw32_t PADS[3];
}pm_regs_t;

constexpr uint32_t PM_PASSWD=(0x5A<<24);
constexpr uint32_t PM_FULL_RESET=0x20;


//====================================
// Power State Coordination Interface
//====================================

constexpr uint64_t PSCI_POWER_OFF=0x84000008;
constexpr uint64_t PSCI_RESET=0x84000009;


//=======
// Reset
//=======

typedef struct
{
rw32_t SET;
rw32_t CLEAR;
rw32_t STATUS;
rw32_t RES[3];
}init_bank_regs_t;

typedef struct
{
init_bank_regs_t INIT_BANK[2];
}reset_regs_t;


//========
// Common
//========

VOID System::Led(BOOL on)
{
GpioHelper::SetPinMode(PIN_ACTLED, PinMode::Output);
GpioHelper::DigitalWrite(PIN_ACTLED, on);
}

VOID System::PowerOff()
{
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

VOID System::Reset(ResetDevice device)
{
auto reset=(reset_regs_t*)AXI_RESET_BASE;
UINT bank=(UINT)device/32;
UINT mask=(UINT)device&0x1F;
io_write(reset->INIT_BANK[bank].SET, mask);
Cpu::Delay(100);
io_write(reset->INIT_BANK[bank].CLEAR, mask);
Cpu::Delay(100);
}

VOID System::Restart()
{
__asm volatile("\
mov	x0, %0\n\
smc	#0\n\
":: "r" (PSCI_RESET));
while(1)
	Cpu::WaitForInterrupt();
}

}}