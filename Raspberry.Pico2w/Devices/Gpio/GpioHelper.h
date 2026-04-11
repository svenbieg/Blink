//==============
// GpioHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//=============
// Gpio-Helper
//=============

class GpioHelper
{
public:
	// Using
	using InterruptHandler=Devices::System::InterruptHandler;

	// Common
	static inline FLOAT AnalogRead(BYTE Pin)
		{
		throw NotImplementedException();
		}
	static inline BOOL DigitalRead(BYTE Pin)
		{
		assert(Pin<=GPIO_PIN_COUNT);
		return DigitalRead((GpioPin)Pin);
		}
	static inline BOOL DigitalRead(GpioPin Pin)
		{
		UINT value;
		__asm inline volatile("mrc p0, #0, %0, c0, c8": "=r" (value));
		return (value&(1U<<(UINT)Pin))!=0;
		}
	static inline VOID DigitalWrite(BYTE Pin, BOOL Value)
		{
		assert(Pin<=GPIO_PIN_COUNT);
		DigitalWrite((GpioPin)Pin, Value);
		}
	static inline VOID DigitalWrite(GpioPin Pin, BOOL Value)
		{
		__asm inline volatile("mcrr p0, #4, %0, %1, c0":: "r" (Pin), "r" (Value));
		}
	static inline VOID SetInterruptHandler(BYTE Pin, VOID (*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		assert(Pin<=GPIO_PIN_COUNT);
		auto gpio=GpioHost::Create();
		gpio->SetInterruptHandler((GpioPin)Pin, Procedure, Mode);
		}
	template <class _owner_t> static inline VOID SetInterruptHandler(BYTE Pin, _owner_t* Owner, VOID (_owner_t::*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		assert(Pin<=GPIO_PIN_COUNT);
		auto gpio=GpioHost::Create();
		gpio->SetInterruptHandler((GpioPin)Pin, Owner, Procedure, Mode);
		}
	static inline VOID SetInterruptHandler(GpioPin Pin, VOID (*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		auto gpio=GpioHost::Create();
		gpio->SetInterruptHandler(Pin, Procedure, Mode);
		}
	template <class _owner_t> static inline VOID SetInterruptHandler(GpioPin Pin, _owner_t* Owner, VOID (_owner_t::*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		auto gpio=GpioHost::Create();
		gpio->SetInterruptHandler(Pin, Owner, Procedure, Mode);
		}
	static inline VOID SetPinMode(BYTE Pin, GpioPinMode Mode, GpioPullMode PullMode=GpioPullMode::None)
		{
		assert(Pin<=GPIO_PIN_COUNT);
		auto gpio=GpioHost::Create();
		gpio->SetPinMode((GpioPin)Pin, Mode, PullMode);
		}
	static inline VOID SetPinMode(GpioPin Pin, GpioPinMode Mode, GpioPullMode PullMode=GpioPullMode::None)
		{
		auto gpio=GpioHost::Create();
		gpio->SetPinMode(Pin, Mode, PullMode);
		}
};

}}