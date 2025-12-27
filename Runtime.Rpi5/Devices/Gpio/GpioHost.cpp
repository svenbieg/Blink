//==============
// GpioHost.cpp
//==============

#include "GpioHost.h"


//=======
// Using
//=======

#include <base.h>
#include <io.h>
#include "Concurrency/Task.h"

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
rw32_t STATUS;
rw32_t CTRL;
}rp1_pin_regs_t;

constexpr uint32_t CTRL_IRQRESET=(1<<28);
constexpr uint32_t CTRL_IRQMASK=(0xF<<20);
constexpr uint32_t CTRL_IRQMASK_EDGE_HIGH=(1<<21);
constexpr uint32_t CTRL_IRQMASK_EDGE_LOW=(1<<20);
constexpr bits32_t CTRL_INOVER={ 3, 16 };
constexpr bits32_t CTRL_OEOVER={ 3, 14 };
constexpr uint32_t CTRL_OEOVER_DISABLE=2;
constexpr uint32_t CTRL_OEOVER_ENABLE=3;
constexpr bits32_t CTRL_OUTOVER={ 3, 12 };
constexpr uint32_t CTRL_OUTOVER_LOW=2;
constexpr uint32_t CTRL_OUTOVER_HIGH=3;
constexpr bits32_t CTRL_FUNCSEL={ 0x1F, 0 };
constexpr uint32_t CTRL_FUNCSEL_DEFAULT=0x1F;

constexpr UINT STATUS_INFILTERED=(1<<18);

typedef struct
{
rw32_t INTE;
rw32_t INTF;
rw32_t INTS;
}rp1_int_regs_t;

typedef struct
{
rp1_pin_regs_t PIN[BANK_PIN_COUNT];
rw32_t INTR;
rp1_int_regs_t PROC[2];
rp1_int_regs_t PCIE;
}rp1_gpio_regs_t;

typedef struct
{
rw32_t VSEL;
rw32_t PIN[BANK_PIN_COUNT];
}rp1_pads_regs_t;

constexpr uint32_t PADS_VSEL_3V3=0;
constexpr uint32_t PADS_VSEL_1V8=1;

constexpr uint32_t PADS_OUTPUT_DISABLE=(1<<7);
constexpr uint32_t PADS_INPUT_ENABLE=(1<<6);
constexpr bits32_t PADS_DRIVE={ 0x3, 3 };
constexpr uint32_t PADS_DRIVE_2MA=0;
constexpr uint32_t PADS_DRIVE_4MA=1;
constexpr uint32_t PADS_DRIVE_8MA=2;
constexpr uint32_t PADS_DRIVE_12MA=3;
constexpr bits32_t PADS_PULL={ 0x3, 2 };
constexpr uint32_t PADS_SCHMITT=(1<<1);
constexpr uint32_t PADS_SLEWFAST=(1<<0);

typedef struct
{
uint32_t RES0;
uint32_t RES1;
rw32_t READ;
uint32_t RES2[1021];
rw32_t XOR;
uint32_t RES3[1023];
rw32_t SET;
uint32_t RES4[1023];
rw32_t CLEAR;
uint32_t RES5[1023];
}rp1_rio_regs_t;


//========
// Common
//========

BOOL GpioHost::DigitalRead(GpioRp1Pin pin)
{
auto rio=(rp1_rio_regs_t*)RP1_RIO0_BASE;
UINT pin_mask=1UL<<(UINT)pin;
return io_read(rio->READ, pin_mask)!=0;
}

VOID GpioHost::SetPinMode(GpioRp1Pin pin, GpioRp1PinMode mode, GpioPullMode pull_mode)
{
auto pads=(rp1_pads_regs_t*)RP1_PADS0_BASE;
auto gpio=(rp1_gpio_regs_t*)RP1_GPIO0_BASE;
UINT id=(UINT)pin;
SpinLock lock(m_CriticalSection);
UINT pad=io_read(pads->PIN[id]);
UINT ctrl=io_read(gpio->PIN[id].CTRL);
bits_set(pad, PADS_PULL, (UINT)pull_mode);
switch(mode)
	{
	case GpioRp1PinMode::Input:
		{
		bits_set(pad, PADS_OUTPUT_DISABLE);
		bits_set(pad, PADS_INPUT_ENABLE);
		bits_set(ctrl, CTRL_OEOVER, CTRL_OEOVER_DISABLE);
		bits_set(ctrl, CTRL_FUNCSEL, CTRL_FUNCSEL_DEFAULT);
		break;
		}
	case GpioRp1PinMode::Output:
		{
		bits_clear(pad, PADS_OUTPUT_DISABLE);
		bits_clear(pad, PADS_INPUT_ENABLE);
		bits_set(ctrl, CTRL_OEOVER, CTRL_OEOVER_ENABLE);
		bits_set(ctrl, CTRL_FUNCSEL, CTRL_FUNCSEL_DEFAULT);
		break;
		}
	default:
		{
		UINT func=(UINT)mode-(UINT)GpioRp1PinMode::Alt0;
		bits_clear(pad, PADS_OUTPUT_DISABLE);
		bits_set(pad, PADS_INPUT_ENABLE);
		bits_set(ctrl, CTRL_INOVER, 0);
		bits_set(ctrl, CTRL_OEOVER, 0);
		bits_set(ctrl, CTRL_OUTOVER, 0);
		bits_set(ctrl, CTRL_FUNCSEL, func);
		break;
		}
	}
io_write(pads->PIN[id], pad);
io_write(gpio->PIN[id].CTRL, ctrl);
}

VOID GpioHost::DigitalWrite(GpioRp1Pin pin, BOOL value)
{
auto rio=(rp1_rio_regs_t*)RP1_RIO0_BASE;
UINT pin_mask=1UL<<(UINT)pin;
io_set(value? rio->SET: rio->CLEAR, pin_mask);
}

VOID GpioHost::SetInterruptHandler(GpioRp1Pin pin, IRQ_HANDLER handler, VOID* param, GpioIrqMode mode)
{
UINT id=(UINT)pin;
UINT pin_mask=1UL<<id;
auto gpio=(rp1_gpio_regs_t*)RP1_GPIO0_BASE;
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
			flags|=CTRL_IRQMASK_EDGE_HIGH;
			flags|=CTRL_IRQMASK_EDGE_LOW;
			break;
			}
		case GpioIrqMode::FallingEdge:
			{
			flags|=CTRL_IRQMASK_EDGE_LOW;
			break;
			}
		case GpioIrqMode::RisingEdge:
			{
			flags|=CTRL_IRQMASK_EDGE_HIGH;
			break;
			}
		}
	io_set(gpio->PIN[id].CTRL, flags);
	io_set(gpio->PCIE.INTE, pin_mask);
	}
else
	{
	io_clear(gpio->PCIE.INTE, pin_mask);
	io_clear(gpio->PIN[id].CTRL, CTRL_IRQMASK);
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
m_IrqMask(0)
{
m_PcieHost=PcieHost::Get();
}

Global<GpioHost> GpioHost::s_Current;


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
auto gpio=(rp1_gpio_regs_t*)RP1_GPIO0_BASE;
UINT mask=gpio->PCIE.INTS;
for(UINT pin=0; mask!=0; pin++)
	{
	if(mask&1)
		{
		if(m_Handlers[pin])
			m_Handlers[pin](m_Parameters[pin]);
		io_set(gpio->PIN[pin].CTRL, CTRL_IRQRESET);
		}
	mask>>=1;
	}
}

}}