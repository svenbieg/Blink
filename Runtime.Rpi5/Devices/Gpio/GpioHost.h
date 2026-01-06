//============
// GpioHost.h
//============

#pragma once


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Pcie/PcieHost.h"
#include "Global.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===========
// Gpio-Host
//===========

class GpioHost: public Global
{
public:
	// Using
	using IRQ_HANDLER=Devices::System::IRQ_HANDLER;
	using PcieHost=Devices::Pcie::PcieHost;

	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<GpioHost> Create() { return Global::Create<GpioHost>(); }

	// Common
	BOOL DigitalRead(GpioRp1Pin Pin);
	VOID DigitalWrite(GpioRp1Pin Pin, BOOL Value);
	VOID SetInterruptHandler(GpioRp1Pin Pin, IRQ_HANDLER Handler, VOID* Parameter=0, GpioIrqMode Mode=GpioIrqMode::Edge);
	VOID SetPinMode(GpioRp1Pin Pin, GpioRp1PinMode Mode, GpioPullMode PullMode=GpioPullMode::None);

private:
	// Con-/Destructors
	GpioHost();

	// Common
	static VOID HandleInterrupt(VOID* Parameter);
	VOID OnInterrupt();
	Concurrency::CriticalSection m_CriticalSection;
	IRQ_HANDLER m_Handlers[RP1_GPIO_PIN_COUNT];
	UINT m_IrqMask;
	VOID* m_Parameters[RP1_GPIO_PIN_COUNT];
	Handle<PcieHost> m_PcieHost;
	Handle<GpioHost> m_This;
};

}}