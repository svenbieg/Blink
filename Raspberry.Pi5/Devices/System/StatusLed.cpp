//===============
// StatusLed.cpp
//===============

#include "StatusLed.h"


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"

using namespace Devices::Gpio;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID StatusLed::Set(BOOL on)
{
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, on);
}


//==========================
// Con-/Destructors Private
//==========================

StatusLed::StatusLed()
{
GpioHelper::SetPinMode(GpioArmPin::ActivityLed, GpioArmPinMode::Output);
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, false);
}

}}