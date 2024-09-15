//==========
// Gpio.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Peripherals.h"
#include "Gpio.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Arm {


//==========
// Settings
//==========

constexpr UINT RP1_GPIO_PIN_COUNT=28;
constexpr UINT ARM_GPIO_PIN_COUNT=26;
constexpr UINT GPIO_PIN_COUNT=54;


//===========
// Registers
//===========

typedef struct
{
UINT RES0[1];
REG DATA0;
REG IODIR0;
}GPIO_REGS;

typedef struct
{
REG FSEL[7];
REG PULL[4];
}PINCTRL_REGS;


//========
// Common
//========

VOID Gpio::DigitalWrite(BYTE pin, BOOL value)
{
assert(pin>=RP1_GPIO_PIN_COUNT);
assert(pin<GPIO_PIN_COUNT);
auto gpio=(GPIO_REGS*)ARM_GPIO1_BASE;
if(pin>32)
	{
	gpio=(GPIO_REGS*)ARM_GPIO2_BASE;
	pin-=32;
	}
UINT mask=1<<pin;
UINT set=value? mask: 0;
Bits::Set(gpio->DATA0, mask, set);
}

VOID Gpio::SetPinMode(BYTE pin, PinMode mode, PullMode pull)
{
assert(pin>=RP1_GPIO_PIN_COUNT);
assert(pin<GPIO_PIN_COUNT);
auto pin_ctrl=(PINCTRL_REGS*)ARM_PINCTRL1_BASE;
UINT fsel_reg=pin/8;
UINT fsel_shift=(pin%8)*4;
BIT_FIELD fsel_bits={ 0xF, fsel_shift };
Bits::Set(pin_ctrl->FSEL[fsel_reg], fsel_bits, (UINT)mode);
pin+=7;
UINT pull_reg=pin/15;
UINT pull_shift=(pin%15)*2;
BIT_FIELD pull_bits={ 0x3, pull_shift };
Bits::Set(pin_ctrl->PULL[pull_reg], pull_bits, (UINT)pull);
if(mode==PinMode::Output)
	{
	auto gpio=(GPIO_REGS*)ARM_GPIO1_BASE;
	if(pin>32)
		{
		gpio=(GPIO_REGS*)ARM_GPIO2_BASE;
		pin-=32;
		}
	Bits::Clear(gpio->IODIR0, 1<<pin);
	}
}

}}
