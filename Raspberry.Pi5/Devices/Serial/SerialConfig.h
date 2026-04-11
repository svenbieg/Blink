//================
// SerialConfig.h
//================

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Pcie/PcieHost.h"
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
using GpioRp1Pin=Devices::Gpio::GpioRp1Pin;
using GpioRp1PinMode=Devices::Gpio::GpioRp1PinMode;
using Rp1Irq=Devices::Pcie::Rp1Irq;
using ResetDevice=Devices::System::ResetDevice;


//===============
// Configuration
//===============

typedef struct
{
SIZE_T BASE;
GpioRp1Pin TX_PIN;
GpioRp1Pin RX_PIN;
GpioRp1PinMode TX_ALT;
GpioRp1PinMode RX_ALT;
Rp1Irq IRQ;
ResetDevice RESET;
}SERIAL_CONFIG;

const SERIAL_CONFIG SERIAL_DEVICES[]=
	{
	{ RP1_UART0_BASE, GpioRp1Pin::Gpio14, GpioRp1Pin::Gpio15, GpioRp1PinMode::Alt4, GpioRp1PinMode::Alt4, Rp1Irq::UArt0, ResetDevice::None },
	{ RP1_UART1_BASE, GpioRp1Pin::Gpio0, GpioRp1Pin::Gpio1, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt1, ResetDevice::None },
	{ RP1_UART2_BASE, GpioRp1Pin::Gpio4, GpioRp1Pin::Gpio5, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt2, ResetDevice::None },
	{ RP1_UART3_BASE, GpioRp1Pin::Gpio8, GpioRp1Pin::Gpio9, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt3, ResetDevice::None },
	{ RP1_UART4_BASE, GpioRp1Pin::Gpio12, GpioRp1Pin::Gpio13, GpioRp1PinMode::Alt2, GpioRp1PinMode::Alt2, Rp1Irq::UArt4, ResetDevice::None }
	};

const UINT SERIAL_CLOCK=Clocks::XOSC_HZ;
const UINT SERIAL_COUNT=TypeHelper::ArraySize(SERIAL_DEVICES);


//=========
// Devices
//=========

enum class SerialDevice
{
Serial0,
Serial1,
Serial2,
Serial3,
Serial4
};

}}