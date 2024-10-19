//============
// System.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Arm/Gpio.h"
#include "Devices/System/Peripherals.h"
#include "System.h"

using namespace Devices::Arm;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//==============
// Activity-Led
//==============

constexpr UINT ACTLED_PIN=41;


//===============
// Power Manager
//===============

typedef struct
{
UINT RES0[6];
REG RSTC;
REG RSTS;
REG WDOG;
UINT RES1;
REG PADS[3];
}PM_REGS;

constexpr UINT PM_PASSWD=(0x5A<<24);
constexpr UINT PM_FULL_RESET=0x20;


//====================================
// Power State Coordination Interface
//====================================

constexpr UINT64 PSCI_POWER_OFF=0x84000008;
constexpr UINT64 PSCI_RESET=0x84000009;


//=======
// Reset
//=======

typedef struct
{
REG SET;
REG CLEAR;
REG STATUS;
UINT RES[3];
}INIT_BANK_REGS;

typedef struct
{
INIT_BANK_REGS INIT_BANK[2];
}RESET_REGS;


//========
// Common
//========

VOID System::Led(BOOL on)
{
using Gpio=Devices::Arm::Gpio;
Gpio::DigitalWrite(ACTLED_PIN, on);
}

Handle<System> System::Open()
{
if(!s_Default)
	s_Default=new System();
return s_Default;
}

VOID System::PowerOff()
{
__asm volatile("\
mov	x0, %0\n\
smc	#0\n\
":: "r" (PSCI_POWER_OFF));
Abort();
}

VOID System::Reset()
{
__asm volatile("\
mov	x0, %0\n\
smc	#0\n\
":: "r" (PSCI_RESET));
Abort();
}

VOID System::Reset(ResetDevice device)
{
auto reset=(RESET_REGS*)AXI_RESET_BASE;
UINT bank=(UINT)device/32;
UINT mask=(UINT)device&0x1F;
Bits::Write(reset->INIT_BANK[bank].SET, mask);
SleepMicroseconds(100);
Bits::Write(reset->INIT_BANK[bank].CLEAR, mask);
SleepMicroseconds(100);
}


//==========================
// Con-/Destructors Private
//==========================

System::System()
{
using Gpio=Devices::Arm::Gpio;
Gpio::SetPinMode(ACTLED_PIN, PinMode::Output);
Gpio::DigitalWrite(ACTLED_PIN, 0);
}


//================
// Common Private
//================

Handle<System> System::s_Default;

}}
