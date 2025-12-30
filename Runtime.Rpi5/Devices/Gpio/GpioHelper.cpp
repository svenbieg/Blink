//================
// GpioHelper.cpp
//================

#include "GpioHelper.h"


//=======
// Using
//=======

#include "Devices/Gpio/GpioHost.h"
#include "Devices/IoHelper.h"
#include <base.h>


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
RW32 RES0[1];
RW32 DATA0;
RW32 IODIR0;
}ARM_GPIO;

typedef struct
{
RW32 FSEL[7];
RW32 PULL[4];
}ARM_PINCTRL;


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
if(pin<RP1_GPIO_PIN_COUNT)
	return DigitalRead((GpioRp1Pin)pin);
return DigitalRead((GpioArmPin)pin);
}

BOOL GpioHelper::DigitalRead(GpioArmPin pin)
{
auto gpio=(ARM_GPIO*)ARM_GPIO1_BASE;
UINT id=(UINT)pin;
if(id>=32)
	{
	gpio=(ARM_GPIO*)ARM_GPIO2_BASE;
	id-=32;
	}
UINT mask=1UL<<id;
return IoHelper::Read(gpio->DATA0, mask)!=0;
}

BOOL GpioHelper::DigitalRead(GpioRp1Pin pin)
{
auto gpio=GpioHost::Get();
return gpio->DigitalRead(pin);
}

VOID GpioHelper::DigitalWrite(BYTE pin, BOOL value)
{
if(pin<RP1_GPIO_PIN_COUNT)
	return DigitalWrite((GpioRp1Pin)pin, value);
return DigitalWrite((GpioArmPin)pin, value);
}

VOID GpioHelper::DigitalWrite(GpioArmPin arm_pin, BOOL value)
{
auto gpio=(ARM_GPIO*)ARM_GPIO1_BASE;
UINT pin=(UINT)arm_pin;
if(pin>=32)
	{
	gpio=(ARM_GPIO*)ARM_GPIO2_BASE;
	pin-=32;
	}
UINT mask=1UL<<pin;
UINT set=value? mask: 0;
IoHelper::Set(gpio->DATA0, mask, set);
}

VOID GpioHelper::DigitalWrite(GpioRp1Pin pin, BOOL value)
{
auto gpio=GpioHost::Get();
return gpio->DigitalWrite(pin, value);
}

VOID GpioHelper::SetInterruptHandler(BYTE pin, IRQ_HANDLER handler, VOID* param, GpioIrqMode mode)
{
if(pin>=RP1_GPIO_PIN_COUNT)
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
if(pin>=RP1_GPIO_PIN_COUNT)
	throw InvalidArgumentException();
return SetPinMode((GpioRp1Pin)pin, (GpioRp1PinMode)mode, pull_mode);
}

VOID GpioHelper::SetPinMode(GpioArmPin arm_pin, GpioArmPinMode mode, GpioPullMode pull_mode)
{
auto pin_ctrl=(ARM_PINCTRL*)ARM_PINCTRL1_BASE;
UINT pin=(UINT)arm_pin;
BYTE sel_reg=pin/8;
BYTE sel_shift=(pin%8)*4;
BITS sel_bits={ 0xF, sel_shift };
IoHelper::Set(pin_ctrl->FSEL[sel_reg], sel_bits, (UINT)mode);
UINT offset=pin+7;
BYTE pull_reg=offset/15;
BYTE pull_shift=(offset%15)*2;
BITS pull_bits={ 0x3, pull_shift };
IoHelper::Set(pin_ctrl->PULL[pull_reg], pull_bits, (UINT)pull_mode);
if(mode==GpioArmPinMode::Output)
	{
	auto gpio=(ARM_GPIO*)ARM_GPIO1_BASE;
	if(pin>32)
		{
		gpio=(ARM_GPIO*)ARM_GPIO2_BASE;
		pin-=32;
		}
	IoHelper::Set(gpio->IODIR0, 1UL<<pin, 0);
	}
}

VOID GpioHelper::SetPinMode(GpioRp1Pin pin, GpioRp1PinMode mode, GpioPullMode pull_mode)
{
auto gpio=GpioHost::Get();
gpio->SetPinMode(pin, mode, pull_mode);
}

}}