//================
// GpioHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include <base.h>
#include <io.h>
#include "Devices/Gpio/GpioHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


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
rw32_t RES0[1];
rw32_t DATA0;
rw32_t IODIR0;
}arm_gpio_t;

typedef struct
{
rw32_t FSEL[7];
rw32_t PULL[4];
}arm_pinctrl_t;


//========
// Common
//========

VOID GpioHelper::DigitalWrite(BYTE pin, BOOL value)
{
assert(pin>=RP1_GPIO_PIN_COUNT);
assert(pin<GPIO_PIN_COUNT);
auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
if(pin>32)
	{
	gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
	pin-=32;
	}
UINT mask=1<<pin;
UINT set=value? mask: 0;
io_write(gpio->DATA0, mask, set);
}

VOID GpioHelper::SetPinMode(BYTE pin, PinMode mode, PullMode pull)
{
assert(pin>=RP1_GPIO_PIN_COUNT);
assert(pin<GPIO_PIN_COUNT);
auto pin_ctrl=(arm_pinctrl_t*)ARM_PINCTRL1_BASE;
BYTE fsel_reg=pin/8;
BYTE fsel_shift=(pin%8)<<2;
bits32_t fsel_bits={ 0xF, fsel_shift };
io_write(pin_ctrl->FSEL[fsel_reg], fsel_bits, (UINT)mode);
pin+=7;
BYTE pull_reg=pin/15;
BYTE pull_shift=(pin%15)<<1;
bits32_t pull_bits={ 0x3, pull_shift };
io_write(pin_ctrl->PULL[pull_reg], pull_bits, (UINT)pull);
if(mode==PinMode::Output)
	{
	auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
	if(pin>32)
		{
		gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
		pin-=32;
		}
	io_write(gpio->IODIR0, 1<<pin, 0);
	}
}

}}