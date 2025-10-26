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
#include "Devices/Gpio/GpioHost.h"


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

FLOAT GpioHelper::AnalogRead(BYTE pin)
{
throw NotImplementedException();
return 0.f;
}

BOOL GpioHelper::DigitalRead(BYTE pin)
{
if(pin<GPIO_PIN_COUNT)
	return DigitalRead((GpioRp1Pin)pin);
return DigitalRead((GpioArmPin)pin);
}

BOOL GpioHelper::DigitalRead(GpioArmPin pin)
{
auto gpio=(arm_gpio_t*)ARM_GPIO1_BASE;
UINT id=(UINT)pin;
if(id>=32)
	{
	gpio=(arm_gpio_t*)ARM_GPIO2_BASE;
	id-=32;
	}
UINT mask=1UL<<id;
return io_read(gpio->DATA0, mask)!=0;
}

BOOL GpioHelper::DigitalRead(GpioRp1Pin pin)
{
auto gpio=GpioHost::Get();
return gpio->DigitalRead(pin);
}

VOID GpioHelper::DigitalWrite(BYTE pin, BOOL value)
{
if(pin<GPIO_PIN_COUNT)
	return DigitalWrite((GpioRp1Pin)pin, value);
return DigitalWrite((GpioArmPin)pin, value);
}

VOID GpioHelper::DigitalWrite(GpioArmPin arm_pin, BOOL value)
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

VOID GpioHelper::DigitalWrite(GpioRp1Pin pin, BOOL value)
{
auto gpio=GpioHost::Get();
return gpio->DigitalWrite(pin, value);
}

VOID GpioHelper::SetInterruptHandler(BYTE pin, IRQ_HANDLER handler, VOID* param, GpioIrqMode mode)
{
if(pin>=GPIO_PIN_COUNT)
	throw InvalidArgumentException();
SetInterruptHandler((GpioRp1Pin)pin, handler, param, mode);
}

VOID GpioHelper::SetInterruptHandler(GpioRp1Pin pin, IRQ_HANDLER handler, VOID* param, GpioIrqMode mode)
{
auto gpio_host=GpioHost::Get();
gpio_host->SetInterruptHandler(pin, handler, param, mode);
}

VOID GpioHelper::SetPinMode(BYTE pin, GpioPinMode mode, GpioPullMode pull_mode)
{
if(pin>=GPIO_PIN_COUNT)
	throw InvalidArgumentException();
return SetPinMode((GpioRp1Pin)pin, (GpioRp1PinMode)mode, pull_mode);
}

VOID GpioHelper::SetPinMode(GpioArmPin arm_pin, GpioArmPinMode mode, GpioPullMode pull_mode)
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
if(mode==GpioArmPinMode::Output)
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

VOID GpioHelper::SetPinMode(GpioRp1Pin pin, GpioRp1PinMode mode, GpioPullMode pull_mode)
{
auto gpio=GpioHost::Get();
gpio->SetPinMode(pin, mode, pull_mode);
}

}}