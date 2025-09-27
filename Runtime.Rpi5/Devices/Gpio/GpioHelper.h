//==============
// GpioHelper.h
//==============

#pragma once

//			|	Func1			Func2		Func3			Func4			Func5			Func6			Func7			Func8
//-------------------------------------------------------------------------------------------------------------------------------------
// GPIO30	|												WIFI_SDIO_CLK
// GPIO31	|												WIFI_SDIO_CMD
// GPIO32	|												WIFI_SDIO_D0
// GPIO33	|								WIFI_SDIO_D1
// GPIO34	|												WIFI_SDIO_D2
// GPIO35	|								WIFI_SDIO_D3


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//======
// Pins
//======

enum class ArmPin
{
WifiOn=28,
WifiSdioClk=30,
WifiSdioCmd=31,
WifiSdioD0=32,
WifiSdioD1=33,
WifiSdioD2=34,
WifiSdioD3=35,
ActivityLed=41
};


//==========
// Pin-Mode
//==========

enum class ArmPinMode
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

enum class ArmPullMode
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
	static BOOL DigitalRead(ArmPin Pin);
	static VOID DigitalWrite(ArmPin Pin, BOOL Value);
	static VOID SetPinMode(ArmPin Pin, ArmPinMode Mode, ArmPullMode PullMode=ArmPullMode::None);
};

}}