//============
// GpioHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalSection.h"
#include "Devices/System/Interrupts.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//==========
// Settings
//==========

const UINT GPIO_PIN_COUNT=30;


//======
// Pins
//======

enum class GpioPin
{
Gpio0, Gpio1, Gpio2, Gpio3,
Gpio4, Gpio5, Gpio6, Gpio7,
Gpio8, Gpio9, Gpio10, Gpio11,
Gpio12, Gpio13, Gpio14, Gpio15,
Gpio16, Gpio17, Gpio18, Gpio19,
Gpio20, Gpio21, Gpio22, Gpio23,
Gpio24, Gpio25, Gpio26, Gpio27,
Gpio28, Gpio29
};


//==========
// Pin-Mode
//==========

enum class GpioPinMode
{
Func0, Func1, Func2, Func3, Func4, Func5, Func6,
Func7, Func8, Func9, Func10, Func11,
Input, Output,
Disabled=31
};

enum class GpioPullMode
{
None,
PullDown,
PullUp
};

enum class GpioIrqMode
{
None=0,
Low=1,
High=2,
Falling=4,
Rising=8,
Edge=12
};

enum class GpioHysteresis
{
None,
SchmittTrigger
};

enum class GpioSlew
{
Default,
Fast
};

enum class GpioStrength
{
_2mA,
_4mA,
_8mA,
_12mA
};


//===========
// GPIO-Host
//===========

class GpioHost: public Global<GpioHost>
{
public:
	// Using
	using CriticalSection=Concurrency::CriticalSection;
	using InterruptHandler=Devices::System::InterruptHandler;

	// Con-/Destructors
	static inline Handle<GpioHost> Create() { return Global::Create(); }

	// Common
	inline BOOL DigitalRead(GpioPin Pin)
		{
		UINT value;
		__asm inline volatile("mrc p0, #0, %0, c0, c8": "=r" (value));
		return (value&(1U<<(UINT)Pin))!=0;
		}
	inline VOID DigitalWrite(GpioPin Pin, BOOL Value)
		{
		__asm inline volatile("mcrr p0, #4, %0, %1, c0":: "r" (Pin), "r" (Value));
		}
	VOID DisableInterrupt(GpioPin Pin);
	VOID EnableInterrupt(GpioPin Pin, GpioIrqMode Mode);
	inline VOID SetInterruptHandler(GpioPin Pin, VOID (*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		auto handler=new Devices::System::InterruptProcedure(Procedure);
		SetInterruptHandlerInternal(Pin, handler, Mode);
		}
	template <class _owner_t> inline VOID SetInterruptHandler(GpioPin Pin, _owner_t* Owner, VOID (_owner_t::*Procedure)(), GpioIrqMode Mode=GpioIrqMode::Edge)
		{
		auto handler=new Devices::System::InterruptMemberProcedure(Owner, Procedure);
		SetInterruptHandlerInternal(Pin, handler, Mode);
		}
	VOID SetPinMode(GpioPin Pin, GpioHysteresis Hysteresis);
	VOID SetPinMode(GpioPin Pin, GpioPinMode Mode);
	VOID SetPinMode(GpioPin Pin, GpioPinMode Mode, GpioPullMode PullMode);
	VOID SetPinMode(GpioPin Pin, GpioSlew Slew);
	VOID SetPinMode(GpioPin Pin, GpioStrength Strength);

private:
	// Con-/Destructors
	friend Object;
	GpioHost();

	// Common
	VOID HandleInterrupt();
	inline VOID OutputEnable(UINT Pin, BOOL Enable)
		{
		__asm inline volatile("mcrr p0, #4, %0, %1, c4" : : "r" (Pin), "r" (Enable));
		}
	VOID SetInterruptHandlerInternal(GpioPin Pin, InterruptHandler* Handler, GpioIrqMode Mode);
	CriticalSection m_CriticalSection;
	InterruptHandler* m_IrqHandlers[GPIO_PIN_COUNT];
};

}}