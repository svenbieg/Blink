//==============
// GpioHost.cpp
//==============

#include "GpioHost.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"
#include "Devices/System/Interrupts.h"
#include "Devices/IoHelper.h"
#include "Devices/Peripherals.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Gpio {


//===========
// Registers
//===========

typedef struct
{
RO32 STATUS;
RW32 CTRL;
}IO_REGS;

const BITS32 IO_CTRL_FNSEL={ 0x1F, 0 };

typedef struct
{
RW32 INTE[6];
RW32 INTF[6];
RW32 INTS[6];
}IRQ_REGS;

typedef struct
{
IO_REGS IO[48];
UINT RES0[32];
RO32 IRQ_SUMMARY_PROC0[2];
RO32 IRQ_SUMMARY_PROC0_NS[2];
RO32 IRQ_SUMMARY_PROC1[2];
RO32 IRQ_SUMMARY_PROC1_NS[2];
RO32 IRQ_SUMMARY_DORM_WAKE[2];
RO32 IRQ_SUMMARY_DORM_WAKE_NS[2];
RW32 INTR[6];
IRQ_REGS IRQ_CTRL_PROC0;
IRQ_REGS IRQ_CTRL_PROC1;
IRQ_REGS IRQ_CTRL_DORM_WAKE;
}IO_BANK_REGS;

typedef struct
{
RW32 VSEL;
RW32 IO[48];
}PADS_BANK_REGS;

const UINT PADS_ISO			=(1<<8);
const UINT PADS_OD			=(1<<7);
const UINT PADS_IE			=(1<<6);
const BITS PADS_DSTRENGTH	={ 0x3, 4 };
const UINT PADS_PULLUP		=(1<<3);
const UINT PADS_PULLDOWN	=(1<<2);
const BITS PADS_SCHMITT		={ 0x1, 1 };
const BITS PADS_SLEW_FAST	={ 0x1, 0 };


//========
// Common
//========

VOID GpioHost::DisableInterrupt(GpioPin pin)
{
UINT id=(UINT)pin;
UINT reg=id/8;
BITS32 bits;
bits.Mask=0xF;
bits.Shift=(id%8)*4;
auto io_bank=(IO_BANK_REGS*)IO_BANK0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(io_bank->IRQ_CTRL_PROC0.INTE[reg], bits, 0);
}

VOID GpioHost::EnableInterrupt(GpioPin pin, GpioIrqMode mode)
{
UINT id=(UINT)pin;
UINT reg=id/8;
BITS32 bits;
bits.Mask=0xF;
bits.Shift=(id%8)*4;
auto io_bank=(IO_BANK_REGS*)IO_BANK0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(io_bank->IRQ_CTRL_PROC0.INTE[reg], bits, (UINT)mode);
}

VOID GpioHost::SetPinMode(GpioPin pin, GpioHysteresis hyst)
{
UINT id=(UINT)pin;
auto pads=(PADS_BANK_REGS*)PADS_BANK0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(pads->IO[id], PADS_SCHMITT, (UINT)hyst);
}

VOID GpioHost::SetPinMode(GpioPin pin, GpioPinMode mode)
{
UINT id=(UINT)pin;
auto pads=(PADS_BANK_REGS*)PADS_BANK0_BASE;
SpinLock lock(m_CriticalSection);
UINT pad_ctrl=IoHelper::Read(pads->IO[id]);
BitHelper::Clear(pad_ctrl, PADS_IE|PADS_OD);
UINT fnsel=0;
switch(mode)
	{
	case GpioPinMode::Input:
		{
		OutputEnable(id, false);
		BitHelper::Set(pad_ctrl, PADS_IE|PADS_OD);
		fnsel=5;
		break;
		}
	case GpioPinMode::Output:
		{
		OutputEnable(id, true);
		BitHelper::Set(pad_ctrl, PADS_IE);
		fnsel=5;
		break;
		}
	default:
		{
		BitHelper::Set(pad_ctrl, PADS_IE);
		fnsel=(UINT)mode;
		break;
		}
	}
IoHelper::Write(pads->IO[id], pad_ctrl);
auto gpio=(IO_BANK_REGS*)IO_BANK0_BASE;
IoHelper::Write(gpio->IO[id].CTRL, fnsel);
IoHelper::Clear(pads->IO[id], PADS_ISO);
}

VOID GpioHost::SetPinMode(GpioPin pin, GpioPinMode mode, GpioPullMode pull_mode)
{
UINT id=(UINT)pin;
auto pads=(PADS_BANK_REGS*)PADS_BANK0_BASE;
SpinLock lock(m_CriticalSection);
UINT pad_ctrl=IoHelper::Read(pads->IO[id]);
BitHelper::Clear(pad_ctrl, PADS_IE|PADS_OD|PADS_PULLDOWN|PADS_PULLUP);
UINT fnsel=0;
switch(mode)
	{
	case GpioPinMode::Input:
		{
		OutputEnable(id, false);
		BitHelper::Set(pad_ctrl, PADS_IE|PADS_OD);
		fnsel=5;
		break;
		}
	case GpioPinMode::Output:
		{
		OutputEnable(id, true);
		BitHelper::Set(pad_ctrl, PADS_IE);
		fnsel=5;
		break;
		}
	default:
		{
		BitHelper::Set(pad_ctrl, PADS_IE);
		fnsel=(UINT)mode;
		break;
		}
	}
switch(pull_mode)
	{
	case GpioPullMode::PullDown:
		{
		BitHelper::Set(pad_ctrl, PADS_PULLDOWN);
		break;
		}
	case GpioPullMode::PullUp:
		{
		BitHelper::Set(pad_ctrl, PADS_PULLUP);
		break;
		}
	}
IoHelper::Write(pads->IO[id], pad_ctrl);
auto gpio=(IO_BANK_REGS*)IO_BANK0_BASE;
IoHelper::Write(gpio->IO[id].CTRL, fnsel);
IoHelper::Clear(pads->IO[id], PADS_ISO);
}

VOID GpioHost::SetPinMode(GpioPin pin, GpioSlew slew)
{
UINT id=(UINT)pin;
auto pads=(PADS_BANK_REGS*)PADS_BANK0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(pads->IO[id], PADS_SLEW_FAST, (UINT)slew);
}

VOID GpioHost::SetPinMode(GpioPin pin, GpioStrength strength)
{
UINT id=(UINT)pin;
auto pads=(PADS_BANK_REGS*)PADS_BANK0_BASE;
SpinLock lock(m_CriticalSection);
IoHelper::Set(pads->IO[id], PADS_DSTRENGTH, (UINT)strength);
}


//==========================
// Con-/Destructors Private
//==========================

GpioHost::GpioHost():
m_IrqHandlers({ nullptr })
{
Interrupts::SetHandler(Irq::IoBank0, this, &GpioHost::HandleInterrupt);
}


//================
// Common Private
//================

VOID GpioHost::HandleInterrupt()
{
auto io_bank=(IO_BANK_REGS*)IO_BANK0_BASE;
const UINT reg_count=(GPIO_PIN_COUNT/8+1);
for(UINT reg=0; reg<reg_count; reg++)
	{
	UINT status=IoHelper::Read(io_bank->IRQ_CTRL_PROC0.INTS[reg]);
	if(status==0)
		continue;
	while(status)
		{
		UINT msb=31-Cpu::CountLeadingZeros(status);
		UINT reg_id=msb/4;
		UINT lsb=reg_id*4;
		UINT pin=(reg*8)+reg_id;
		UINT pin_status=(status>>lsb)&0xF;
		UINT mask=pin_status<<lsb;
		IoHelper::Write(io_bank->INTR[reg], mask);
		if(m_IrqHandlers[pin])
			m_IrqHandlers[pin]->Run();
		BitHelper::Clear(status, mask);
		}
	}
}

VOID GpioHost::SetInterruptHandlerInternal(GpioPin pin, InterruptHandler* handler, GpioIrqMode mode)
{
DisableInterrupt(pin);
UINT id=(UINT)pin;
if(m_IrqHandlers[id])
	delete m_IrqHandlers[id];
m_IrqHandlers[id]=handler;
if(!handler)
	mode=GpioIrqMode::None;
if(mode!=GpioIrqMode::None)
	EnableInterrupt(pin, mode);
}

}}