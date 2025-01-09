//==========
// Gpio.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include "Devices/System/Cpu.h"
#include "Devices/System/Peripherals.h"
#include "BitHelper.h"
#include "Gpio.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//======
// Pins
//======

constexpr UINT GPIO_PIN_COUNT=28;


//===========
// Registers
//===========

typedef struct
{
REG STATUS;
REG CTRL;
}PIN_REGS;

constexpr UINT GPIO_CTRL_INOVER=(3<<16);
constexpr UINT GPIO_CTRL_OEOVER=(3<<14);
constexpr UINT GPIO_CTRL_OEOVER_DISABLE=(2<<14);
constexpr UINT GPIO_CTRL_OUTOVER=(3<<12);
constexpr UINT GPIO_CTRL_OUTOVER_LOW=(2<<12);
constexpr UINT GPIO_CTRL_FUNCSEL=(0x1F<<0);

constexpr UINT GPIO_STATUS_INFILTERED=(1<<18);

typedef struct
{
REG INTE;
REG INTF;
REG INTS;
}INT_REGS;

typedef struct
{
PIN_REGS PIN[GPIO_PIN_COUNT];
REG INTR;
INT_REGS PROC[2];
INT_REGS PCIE;
}GPIO_REGS;


//======
// Pads
//======

typedef struct
{
REG VSEL;
REG PIN[GPIO_PIN_COUNT];
}PADS_REGS;

constexpr UINT PADS_VSEL_3V3=0;
constexpr UINT PADS_VSEL_1V8=1;

constexpr UINT PADS_OUTPUT_DISABLE=(1<<7);
constexpr UINT PADS_INPUT_ENABLE=(1<<6);
constexpr BIT_FIELD PADS_DRIVE={ 0x3, 3 };
constexpr UINT PADS_DRIVE_2MA=0;
constexpr UINT PADS_DRIVE_4MA=1;
constexpr UINT PADS_DRIVE_8MA=2;
constexpr UINT PADS_DRIVE_12MA=3;
constexpr BIT_FIELD PADS_PULL={ 0x3, 2 };
constexpr UINT PADS_SCHMITT=(1<<1);
constexpr UINT PADS_SLEWFAST=(1<<0);


//========
// Common
//========

BOOL Gpio::DigitalRead(BYTE pin)
{
assert(pin<GPIO_PIN_COUNT);
GPIO_REGS* gpio=(GPIO_REGS*)RP1_GPIO0_BASE;
REG& status_reg=gpio->PIN[pin].STATUS;
UINT value=BitHelper::Get(status_reg, GPIO_STATUS_INFILTERED);
return value!=0;
}

VOID Gpio::DigitalWrite(BYTE pin, BOOL set)
{
assert(pin<GPIO_PIN_COUNT);
GPIO_REGS* gpio=(GPIO_REGS*)RP1_GPIO0_BASE;
REG& ctrl_reg=gpio->PIN[pin].CTRL;
if(set)
	{
	BitHelper::Set(ctrl_reg, GPIO_CTRL_OUTOVER);
	}
else
	{
	BitHelper::Set(ctrl_reg, GPIO_CTRL_OUTOVER, GPIO_CTRL_OUTOVER_LOW);
	}
}

VOID Gpio::SetPinMode(BYTE pin, PinMode mode, PullMode pull)
{
assert(pin<GPIO_PIN_COUNT);
PADS_REGS* pads_bank=(PADS_REGS*)RP1_PADS0_BASE;
GPIO_REGS* gpio=(GPIO_REGS*)RP1_GPIO0_BASE;
REG& pad_reg=pads_bank->PIN[pin];
REG& ctrl_reg=gpio->PIN[pin].CTRL;
UINT pad=BitHelper::Get(pad_reg);
UINT ctrl=BitHelper::Get(ctrl_reg);
BitHelper::Set(pad, PADS_PULL, (UINT)pull);
switch(mode)
	{
	case PinMode::Input:
		{
		BitHelper::Set(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Set(pad, PADS_INPUT_ENABLE);
		BitHelper::Set(ctrl, GPIO_CTRL_OEOVER, GPIO_CTRL_OEOVER_DISABLE);
		BitHelper::Set(ctrl, GPIO_CTRL_FUNCSEL);
		break;
		}
	case PinMode::Output:
		{
		BitHelper::Clear(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Clear(pad, PADS_INPUT_ENABLE);
		BitHelper::Set(ctrl, GPIO_CTRL_OEOVER|GPIO_CTRL_FUNCSEL);
		break;
		}
	default:
		{
		UINT func=(UINT)mode-(UINT)PinMode::Alt0;
		BitHelper::Clear(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Set(pad, PADS_INPUT_ENABLE);
		BitHelper::Clear(ctrl, GPIO_CTRL_INOVER|GPIO_CTRL_OEOVER|GPIO_CTRL_OUTOVER|GPIO_CTRL_FUNCSEL);
		BitHelper::Set(ctrl, func);
		break;
		}
	}
BitHelper::Write(pad_reg, pad);
BitHelper::Write(ctrl_reg, ctrl);
}

VOID Gpio::SetPullMode(BYTE pin, PullMode pull)
{
assert(pin<GPIO_PIN_COUNT);
PADS_REGS* pads_bank=(PADS_REGS*)RP1_PADS0_BASE;
BitHelper::Set(pads_bank->PIN[pin], PADS_PULL, (UINT)pull);
}

}}
