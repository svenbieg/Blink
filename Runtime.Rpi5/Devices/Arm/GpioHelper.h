//==============
// GpioHelper.h
//==============

#pragma once


//===========
// Namespace
//===========

namespace Devices {
	namespace Arm {


//==========
// Pin-Mode
//==========

enum class PinMode
{
Output,
Func1,
Func2,
Func3,
Func4,
Func5,
Func6,
Func7,
Func8
};


//===========
// Pull-Mode
//===========

enum class PullMode
{
None,
PullDown,
PullUp
};


//======
// Gpio
//======

class GpioHelper
{
public:
	// Common
	static VOID DigitalWrite(BYTE Pin, BOOL Value);
	static VOID SetPinMode(BYTE Pin, PinMode Mode, PullMode PullMode=PullMode::None);
};

}}
