//================
// GpioHelper.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <base.h>
#include <io.h>
#include "Devices/Gpio/GpioHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===============
// ARM-Registers
//===============

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

BOOL GpioHelper::DigitalRead(ArmPin arm_pin)
{
auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
UINT pin=(UINT)arm_pin;
if(pin>=32)
	{
	gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
	pin-=32;
	}
UINT mask=1UL<<pin;
return io_read(gpio->DATA0, mask)!=0;
}

VOID GpioHelper::DigitalWrite(ArmPin arm_pin, BOOL value)
{
auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
UINT pin=(UINT)arm_pin;
if(pin>=32)
	{
	gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
	pin-=32;
	}
UINT mask=1UL<<pin;
UINT set=value? mask: 0;
io_write(gpio->DATA0, mask, set);
}

VOID GpioHelper::SetPinMode(ArmPin arm_pin, ArmPinMode mode, ArmPullMode pull_mode)
{
auto pin_ctrl=(arm_pinctrl_t*)ARM_PINCTRL1_BASE;
UINT pin=(UINT)arm_pin;
BYTE sel_reg=pin/8;
BYTE sel_shift=(pin%8)*4;
bits32_t sel_bits={ 0xF, sel_shift };
io_write(pin_ctrl->FSEL[sel_reg], sel_bits, (UINT)mode);
UINT offset=pin+7;
BYTE pull_reg=offset/15;
BYTE pull_shift=(offset%15)*2;
bits32_t pull_bits={ 0x3, pull_shift };
io_write(pin_ctrl->PULL[pull_reg], pull_bits, (UINT)pull_mode);
if(mode==ArmPinMode::Output)
	{
	auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
	if(pin>32)
		{
		gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
		pin-=32;
		}
	io_write(gpio->IODIR0, 1UL<<pin, 0);
	}
}

}}