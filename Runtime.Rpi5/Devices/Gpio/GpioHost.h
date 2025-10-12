//============
// GpioHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Pcie/PcieHost.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//==========
// Settings
//==========

constexpr UINT GPIO_PIN_COUNT=28;


//===========
// Gpio-Host
//===========

class GpioHost: public Object
{
public:
	// Using
	using IRQ_HANDLER=Devices::System::IRQ_HANDLER;
	using PcieHost=Devices::Pcie::PcieHost;

	// Con-/Destructors
	~GpioHost();
	static Handle<GpioHost> Create();

	// Common
	BOOL DigitalRead(GpioRp1Pin Pin);
	VOID DigitalWrite(GpioRp1Pin Pin, BOOL Value);
	VOID SetInterruptHandler(GpioRp1Pin Pin, IRQ_HANDLER Handler, VOID* Parameter=0, GpioIrqMode Mode=GpioIrqMode::Edge);
	VOID SetPinMode(GpioRp1Pin Pin, GpioRp1PinMode Mode, GpioPullMode PullMode=GpioPullMode::None);

private:
	// Con-/Destructors
	GpioHost();
	static Handle<GpioHost> s_Current;
	static Concurrency::Mutex s_Mutex;

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	Concurrency::CriticalSection m_CriticalSection;
	IRQ_HANDLER m_Handlers[GPIO_PIN_COUNT];
	UINT m_IrqMask;
	VOID* m_Parameters[GPIO_PIN_COUNT];
	Handle<PcieHost> m_PcieHost;
};

}}