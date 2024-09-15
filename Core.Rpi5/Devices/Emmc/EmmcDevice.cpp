//================
// EmmcDevice.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "EmmcDevice.h"
#include "EmmcRegisters.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//==========
// Settings
//==========

constexpr UINT EMMC_CLOCK_HZ=54000000;
constexpr UINT EMMC_INIT_HZ=400000;


//==================
// Con-/Destructors
//==================

EmmcDevice::EmmcDevice(SIZE_T addr, UINT irq):
uAddress(addr),
uInterrupt(irq),
uStatus(EmmcStatus::Reset)
{}


//========
// Common
//========

VOID EmmcDevice::Initialize()
{
ScopedLock lock(cMutex);
uStatus=EmmcStatus::Reset;
auto emmc=(EMMC_REGS*)uAddress;
Bits::Write(emmc->CTRL1, CTRL1::RESET_HOST);
Sleep(10);
if(Bits::Get(emmc->CTRL1, CTRL1::RESET_HOST))
	throw DeviceNotReadyException();
Bits::Write(emmc->CTRL1, CTRL1::RESET_DATA);
Sleep(10);
Bits::Write(emmc->CTRL1, 0);
Bits::Set(emmc->CTRL0, CTRL0::POWER, CTRL0::POWER_VDD1);
Sleep(2);
if(!SetClockSpeed(EMMC_INIT_HZ))
	throw DeviceNotReadyException();
Bits::Write(emmc->IRQ_EN, 0);
Bits::Write(emmc->IRQ, UINT_MAX);
Bits::Write(emmc->IRQ_MASK, UINT_MAX);
// Interrupts::Enable()
}

Handle<Task> EmmcDevice::InitializeAsync()
{
return CreateTask(this, &EmmcDevice::Initialize);
}


//================
// Common Private
//================

BOOL EmmcDevice::SetClockSpeed(UINT hz)
{
auto emmc=(EMMC_REGS*)uAddress;
UINT div=EMMC_CLOCK_HZ/(hz<<1);
if(EMMC_CLOCK_HZ/(div<<1)>hz)
	div++;
UINT ctrl1=CTRL1::CLK_EN|CTRL1::CLK_INTLEN;
Bits::Set(ctrl1, CTRL1::DATA_DTO, CTRL1::DATA_DTO_DEFAULT);
Bits::Set(ctrl1, CTRL1::CLK_FREQ_LO, div);
Bits::Set(ctrl1, CTRL1::CLK_FREQ_HI, div>>8);
Bits::Write(emmc->CTRL1, ctrl1);
UINT64 timeout=GetTickCount64()+1000;
while(!Bits::Get(emmc->CTRL1, CTRL1::CLK_STABLE))
	{
	Sleep(1);
	if(GetTickCount64()>=timeout)
		return false;
	}
return true;
}

}}
