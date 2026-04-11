//==============
// Interrupts.h
//==============

#pragma once


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/InterruptHandler.h"
#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//======
// IRQs
//======

enum class Irq
{
Timer0=0,
Timer0_1=1,
Timer0_2=2,
Timer0_3=3,
Timer1=4,
Timer1_1=5,
Timer1_2=6,
Timer1_3=7,
Pwm0=8,
Pwm1=9,
Dma0=10,
Dma1=11,
Dma2=12,
Dma3=13,
UsbCtrl=14,
Pio0_0=15,
Pio0_1=16,
Pio1_0=17,
Pio1_1=18,
Pio2_0=19,
Pio2_1=20,
IoBank0=21,
IoBank0_NS=22,
IoQspi=23,
IoQspi_NS=24,
SioFifo=25,
SioBell=26,
SioFifo_NS=27,
SioBell_NS=28,
SioTimeCmp=29,
Clocks=30,
Spi0=31,
Spi1=32,
UArt0=33,
UArt1=34,
AdcFifo=35,
I2c0=36,
I2c1=37,
Opt=38,
Trng=39,
Proc0=40,
Proc1=41,
PllSys=42,
PllUsb=43,
PowmanPow=44,
PowmanTimer=45,
TaskSwitch=-1
};


//============
// IRQ-Target
//============

enum class IrqTarget
{
All=0x3,
Secondary=0x2,
Core0=(1<<0),
Core1=(1<<1)
};


//============
// Interrupts
//============

class Interrupts
{
public:
	// Settings
	static const UINT IRQ_COUNT=52;

	// Common
	static BOOL Active()noexcept;
	static VOID Disable()noexcept;
	static VOID Disable(Irq Irq);
	static VOID Enable()noexcept;
	static VOID Enable(Irq Irq);
	static VOID HandleInterrupt(UINT Irq)noexcept;
	static VOID HandleTaskSwitch()noexcept;
	static VOID Initialize()noexcept;
	static VOID Send(Irq Irq, UINT Core);
	static VOID SetHandler(Irq Irq, VOID (*Procedure)());
	template <class _owner_t> static inline VOID SetHandler(Irq Irq, _owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new InterruptMemberProcedure(Owner, Procedure);
		SetHandlerInternal(Irq, handler);
		}

private:
	// Common
	static VOID SetHandlerInternal(Irq Irq, InterruptHandler* Handler)noexcept;
	static BOOL s_Active[Cpu::CPU_COUNT];
	static UINT s_DisableCount[Cpu::CPU_COUNT];
	static InterruptHandler* s_IrqHandler[IRQ_COUNT];
};

}}