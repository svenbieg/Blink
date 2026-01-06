//==============
// GpioHost.cpp
//==============

#include "GpioHost.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/IoHelper.h"

using namespace Concurrency;
using namespace Devices::Pcie;


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===========
// Registers
//===========

constexpr UINT BANK_PIN_COUNT=32;

typedef struct
{
RW32 STATUS;
RW32 CTRL;
}RP1_PIN_REGS;

constexpr UINT CTRL_IRQRESET=(1<<28);
constexpr UINT CTRL_IRQMASK=(0xF<<20);
constexpr UINT CTRL_IRQMASK_EDGE_HIGH=(1<<21);
constexpr UINT CTRL_IRQMASK_EDGE_LOW=(1<<20);
constexpr BITS CTRL_INOVER={ 3, 16 };
constexpr BITS CTRL_OEOVER={ 3, 14 };
constexpr UINT CTRL_OEOVER_DISABLE=2;
constexpr UINT CTRL_OEOVER_ENABLE=3;
constexpr BITS CTRL_OUTOVER={ 3, 12 };
constexpr UINT CTRL_OUTOVER_LOW=2;
constexpr UINT CTRL_OUTOVER_HIGH=3;
constexpr BITS CTRL_FUNCSEL={ 0x1F, 0 };
constexpr UINT CTRL_FUNCSEL_DEFAULT=0x1F;

constexpr UINT STATUS_INFILTERED=(1<<18);

typedef struct
{
RW32 INTE;
RW32 INTF;
RW32 INTS;
}RP1_INT_REGS;

typedef struct
{
RP1_PIN_REGS PIN[BANK_PIN_COUNT];
RW32 INTR;
RP1_INT_REGS PROC[2];
RP1_INT_REGS PCIE;
}RP1_GPIO_REGS;

typedef struct
{
RW32 VSEL;
RW32 PIN[BANK_PIN_COUNT];
}RP1_PADS_REGS;

constexpr UINT PADS_VSEL_3V3=0;
constexpr UINT PADS_VSEL_1V8=1;

constexpr UINT PADS_OUTPUT_DISABLE=(1<<7);
constexpr UINT PADS_INPUT_ENABLE=(1<<6);
constexpr BITS PADS_DRIVE={ 0x3, 3 };
constexpr UINT PADS_DRIVE_2MA=0;
constexpr UINT PADS_DRIVE_4MA=1;
constexpr UINT PADS_DRIVE_8MA=2;
constexpr UINT PADS_DRIVE_12MA=3;
constexpr BITS PADS_PULL={ 0x3, 2 };
constexpr UINT PADS_SCHMITT=(1<<1);
constexpr UINT PADS_SLEWFAST=(1<<0);

typedef struct
{
RW32 RES0;
RW32 RES1;
RW32 READ;
RW32 RES2[1021];
RW32 XOR;
RW32 RES3[1023];
RW32 SET;
RW32 RES4[1023];
RW32 CLEAR;
RW32 RES5[1023];
}RP1_RIO_REGS;


//========
// Common
//========

BOOL GpioHost::DigitalRead(GpioRp1Pin pin)
{
auto rio=(RP1_RIO_REGS*)RP1_RIO0_BASE;
UINT pin_mask=1UL<<(UINT)pin;
return IoHelper::Read(rio->READ, pin_mask)!=0;
}

VOID GpioHost::SetPinMode(GpioRp1Pin pin, GpioRp1PinMode mode, GpioPullMode pull_mode)
{
auto pads=(RP1_PADS_REGS*)RP1_PADS0_BASE;
auto gpio=(RP1_GPIO_REGS*)RP1_GPIO0_BASE;
UINT id=(UINT)pin;
SpinLock lock(m_CriticalSection);
UINT pad=IoHelper::Read(pads->PIN[id]);
UINT ctrl=IoHelper::Read(gpio->PIN[id].CTRL);
BitHelper::Set(pad, PADS_PULL, (UINT)pull_mode);
switch(mode)
	{
	case GpioRp1PinMode::Input:
		{
		BitHelper::Set(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Set(pad, PADS_INPUT_ENABLE);
		BitHelper::Set(ctrl, CTRL_OEOVER, CTRL_OEOVER_DISABLE);
		BitHelper::Set(ctrl, CTRL_FUNCSEL, CTRL_FUNCSEL_DEFAULT);
		break;
		}
	case GpioRp1PinMode::Output:
		{
		BitHelper::Clear(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Clear(pad, PADS_INPUT_ENABLE);
		BitHelper::Set(ctrl, CTRL_OEOVER, CTRL_OEOVER_ENABLE);
		BitHelper::Set(ctrl, CTRL_FUNCSEL, CTRL_FUNCSEL_DEFAULT);
		break;
		}
	default:
		{
		UINT func=(UINT)mode-(UINT)GpioRp1PinMode::Alt0;
		BitHelper::Clear(pad, PADS_OUTPUT_DISABLE);
		BitHelper::Set(pad, PADS_INPUT_ENABLE);
		BitHelper::Set(ctrl, CTRL_INOVER, 0);
		BitHelper::Set(ctrl, CTRL_OEOVER, 0);
		BitHelper::Set(ctrl, CTRL_OUTOVER, 0);
		BitHelper::Set(ctrl, CTRL_FUNCSEL, func);
		break;
		}
	}
IoHelper::Write(pads->PIN[id], pad);
IoHelper::Write(gpio->PIN[id].CTRL, ctrl);
}

VOID GpioHost::DigitalWrite(GpioRp1Pin pin, BOOL value)
{
auto rio=(RP1_RIO_REGS*)RP1_RIO0_BASE;
UINT pin_mask=1UL<<(UINT)pin;
IoHelper::Set(value? rio->SET: rio->CLEAR, pin_mask);
}

VOID GpioHost::SetInterruptHandler(GpioRp1Pin pin, IRQ_HANDLER handler, VOID* param, GpioIrqMode mode)
{
UINT id=(UINT)pin;
UINT pin_mask=1UL<<id;
auto gpio=(RP1_GPIO_REGS*)RP1_GPIO0_BASE;
SpinLock lock(m_CriticalSection);
if(handler)
	{
	UINT irq_mask=m_IrqMask;
	FlagHelper::Set(m_IrqMask, pin_mask);
	m_Handlers[id]=handler;
	m_Parameters[id]=param;
	if(irq_mask==0)
		m_PcieHost->SetInterruptHandler(Rp1Irq::IoBank0, &GpioHost::HandleInterrupt, this);
	UINT flags=0;
	switch(mode)
		{
		case GpioIrqMode::Edge:
			{
			FlagHelper::Set(flags, CTRL_IRQMASK_EDGE_HIGH);
			FlagHelper::Set(flags, CTRL_IRQMASK_EDGE_LOW);
			break;
			}
		case GpioIrqMode::FallingEdge:
			{
			FlagHelper::Set(flags, CTRL_IRQMASK_EDGE_LOW);
			break;
			}
		case GpioIrqMode::RisingEdge:
			{
			FlagHelper::Set(flags, CTRL_IRQMASK_EDGE_HIGH);
			break;
			}
		}
	IoHelper::Write(gpio->PIN[id].CTRL, flags);
	IoHelper::Set(gpio->PCIE.INTE, pin_mask);
	}
else
	{
	IoHelper::Clear(gpio->PCIE.INTE, pin_mask);
	IoHelper::Clear(gpio->PIN[id].CTRL, CTRL_IRQMASK);
	FlagHelper::Clear(m_IrqMask, pin_mask);
	m_Handlers[id]=nullptr;
	m_Parameters[id]=nullptr;
	if(m_IrqMask==0)
		m_PcieHost->SetInterruptHandler(Rp1Irq::IoBank0, nullptr);
	}
}


//==========================
// Con-/Destructors Private
//==========================

GpioHost::GpioHost():
m_IrqMask(0),
m_This(this)
{
m_PcieHost=PcieHost::Create();
}


//================
// Common Private
//================

VOID GpioHost::HandleInterrupt(VOID* param)
{
auto gpio_host=(GpioHost*)param;
gpio_host->OnInterrupt();
}

VOID GpioHost::OnInterrupt()
{
auto gpio=(RP1_GPIO_REGS*)RP1_GPIO0_BASE;
UINT mask=gpio->PCIE.INTS;
for(UINT pin=0; mask!=0; pin++)
	{
	if(mask&1)
		{
		if(m_Handlers[pin])
			m_Handlers[pin](m_Parameters[pin]);
		IoHelper::Set(gpio->PIN[pin].CTRL, CTRL_IRQRESET);
		}
	mask>>=1;
	}
}

}}