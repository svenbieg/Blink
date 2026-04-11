//================
// SerialConfig.h
//================

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/Clocks.h"
#include <base.h>


//===========
// Namespace
//===========

namespace Devices {
	namespace Serial {


//=======
// Using
//=======

using Clocks=Devices::Timers::Clocks;
using GpioPin=Devices::Gpio::GpioPin;
using GpioPinMode=Devices::Gpio::GpioPinMode;
using Irq=Devices::System::Irq;
using ResetDevice=Devices::System::ResetDevice;


//===============
// Configuration
//===============

typedef struct
{
SIZE_T BASE;
GpioPin TX_PIN;
GpioPin RX_PIN;
GpioPinMode TX_ALT;
GpioPinMode RX_ALT;
Irq IRQ;
ResetDevice RESET;
}SERIAL_CONFIG;

const SERIAL_CONFIG SERIAL_DEVICES[]=
	{
	{ UART0_BASE, GpioPin::Gpio0, GpioPin::Gpio1, GpioPinMode::Func2, GpioPinMode::Func2, Irq::UArt0, ResetDevice::UArt0 },
	{ UART1_BASE, GpioPin::Gpio4, GpioPin::Gpio5, GpioPinMode::Func2, GpioPinMode::Func2, Irq::UArt1, ResetDevice::UArt1 },
	};

const UINT SERIAL_CLOCK=Clocks::PERI_CLK_HZ;
const UINT SERIAL_COUNT=TypeHelper::ArraySize(SERIAL_DEVICES);


//=========
// Devices
//=========

enum class SerialDevice
{
Serial0,
Serial1
};

}}