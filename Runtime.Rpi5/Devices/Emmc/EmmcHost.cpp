//==============
// EmmcHost.cpp
//==============

#include "EmmcHost.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/IoHelper.h"

using namespace Concurrency;
using namespace Devices::System;
using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//==================
// Con-/Destructors
//==================

EmmcHost::~EmmcHost()
{
PowerOff();
}


//========
// Common
//========

UINT EmmcHost::AppCommand(UINT cmd, UINT arg, UINT timeout)
{
UINT response[4];
Command(CMD_APPCMD, m_RelativeCardAddress<<16);
Command(cmd, arg, response, nullptr, 0, 0, timeout);
return response[0];
}

VOID EmmcHost::AppCommand(UINT cmd, UINT arg, UINT* response, UINT timeout)
{
Command(CMD_APPCMD, m_RelativeCardAddress<<16);
Command(cmd, arg, response, nullptr, 0, 0, timeout);
}

VOID EmmcHost::AppCommand(UINT cmd, UINT arg, UINT* response, UINT* buf, UINT block_count, UINT block_size, UINT timeout)
{
Command(CMD_APPCMD, m_RelativeCardAddress<<16);
Command(cmd, arg, response, buf, block_size, block_count, timeout);
}

UINT EmmcHost::Command(UINT cmd, UINT arg, UINT timeout)
{
UINT response[4];
Command(cmd, arg, response, nullptr, 0, 0, timeout);
return response[0];
}

VOID EmmcHost::Command(UINT cmd, UINT arg, UINT* response, UINT timeout)
{
Command(cmd, arg, response, nullptr, 0, 0, timeout);
}

VOID EmmcHost::Command(UINT cmd, UINT arg, UINT* response, UINT* buf, UINT block_count, UINT block_size, UINT timeout)
{
assert(block_count<0x10000);
auto emmc=(EMMC_REGS*)m_Address;
SpinLock lock(m_CriticalSection);
if(IoHelper::Read(emmc->STATUS, STATUS_CMD_INHIBIT))
	throw DeviceNotReadyException();
if(IoHelper::Read(emmc->STATUS, STATUS_DAT_INHIBIT))
	{
	IoHelper::Set(emmc->CTRL1, CTRL1_RESET_DATA);
	for(UINT retry=0; retry>=0; retry++)
		{
		if(IoHelper::Read(emmc->STATUS, STATUS_DAT_INHIBIT)==0)
			break;
		if(retry==10)
			throw DeviceNotReadyException();
		}
	}
IoHelper::Write(emmc->BLKSIZECNT, (block_count<<16)|block_size);
IoHelper::Write(emmc->ARG1, arg);
IoHelper::Write(emmc->CMD, cmd);
m_IrqFlags=IRQF_CMD_DONE|IRQF_ERR;
m_Signal.Wait(lock, timeout);
UINT resp_type=BitHelper::Get(cmd, CMF_RSPNS_MASK);
if(response)
	{
	switch(resp_type)
		{
		case CMF_RSPNS_136:
			{
			response[0]=IoHelper::Read(emmc->RESP[0]);
			response[1]=IoHelper::Read(emmc->RESP[1]);
			response[2]=IoHelper::Read(emmc->RESP[2]);
			response[3]=IoHelper::Read(emmc->RESP[3]);
			break;
			}
		case CMF_RSPNS_48:
		case CMF_RSPNS_48_BUSY:
			{
			response[0]=IoHelper::Read(emmc->RESP[0]);
			break;
			}
		default:
			break;
		}
	}
if(!FlagHelper::Get(cmd, CMF_ISDATA))
	return;
UINT size=block_count*block_size;
UINT pos=0;
if(FlagHelper::Get(cmd, CMF_DAT_DIR_READ))
	{
	for(UINT block=0; block<block_count; block++)
		{
		if(!FlagHelper::Get(emmc->STATUS, STATUS_READ_AVAILABLE))
			{
			m_IrqFlags=IRQF_READ_RDY|IRQF_ERR;
			m_Signal.Wait(lock, timeout);
			}
		UINT copy=TypeHelper::Min(size-pos, block_size);
		auto end=buf+(copy/sizeof(UINT));
		while(buf<end)
			*buf++=emmc->DATA;
		pos+=copy;
		}
	}
else
	{
	for(UINT block=0; block<block_count; block++)
		{
		if(!FlagHelper::Get(emmc->STATUS, STATUS_WRITE_AVAILABLE))
			{
			m_IrqFlags=IRQF_WRITE_RDY|IRQF_ERR;
			m_Signal.Wait(lock, timeout);
			}
		UINT copy=TypeHelper::Min(size-pos, block_size);
		auto end=buf+(copy/sizeof(UINT));
		while(buf<end)
			emmc->DATA=*buf++;
		pos+=copy;
		}
	}
if(!FlagHelper::Get(m_IrqFlags, IRQF_DATA_DONE))
	{
	m_IrqFlags=IRQF_DATA_DONE|IRQF_ERR;
	m_Signal.Wait(lock, timeout);
	}
}

VOID EmmcHost::PollRegister(UINT fn, UINT addr, BYTE mask, BYTE value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, fn);
BitHelper::Set(arg, IO_RW_ADDR, addr);
UINT response=0;
do
	{
	Command(CMD_IO_RW_DIRECT, arg, &response);
	if(BitHelper::Get((BYTE)response, mask)==value)
		return;
	Task::Sleep(2);
	}
while(SystemTimer::GetTickCount64()<=end);
throw TimeoutException();
}

VOID EmmcHost::PowerOff()
{
Interrupts::SetHandler(m_Irq, nullptr);
auto emmc=(EMMC_REGS*)m_Address;
IoHelper::Write(emmc->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
}

BYTE EmmcHost::ReadRegister(UINT fn, UINT addr)
{
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, fn);
BitHelper::Set(arg, IO_RW_ADDR, addr);
UINT value=0;
Command(CMD_IO_RW_DIRECT, arg, &value);
return (BYTE)value;
}

VOID EmmcHost::Reset()
{
Interrupts::SetHandler(m_Irq, nullptr);
auto emmc=(EMMC_REGS*)m_Address;
IoHelper::Write(emmc->CTRL1, CTRL1_RESET_HOST);
Task::Sleep(10);
if(IoHelper::Read(emmc->CTRL1, CTRL1_RESET_ALL))
	throw DeviceNotReadyException();
IoHelper::Write(emmc->CTRL0, CTRL0_POWER, CTRL0_POWER_VDD1);
Task::Sleep(2);
Interrupts::SetHandler(m_Irq, HandleInterrupt, this);
IoHelper::Write(emmc->IRQ, UINT_MAX);
IoHelper::Write(emmc->IRQ_MASK, IRQF_MASK_DEFAULT);
IoHelper::Write(emmc->IRQ_EN, IRQF_MASK_DEFAULT);
Task::Sleep(2);
}

VOID EmmcHost::SelectCard(UINT rca)
{
m_RelativeCardAddress=rca;
Command(CMD_SELECT_CARD, rca<<16);
}

VOID EmmcHost::SetClockRate(UINT base_clock, UINT clock_rate)
{
auto emmc=(EMMC_REGS*)m_Address;
UINT ctrl1=IoHelper::Read(emmc->CTRL1);
if(BitHelper::Get(ctrl1, CTRL1_CLK_EN))
	{
	BitHelper::Set(ctrl1, CTRL1_CLK_INTLEN|CTRL1_CLK_EN, 0);
	IoHelper::Write(emmc->CTRL1, ctrl1);
	Task::Sleep(2);
	}
UINT div=base_clock/(clock_rate<<1);
BitHelper::Set(ctrl1, CTRL1_CLK_FREQ_LO, div);
BitHelper::Set(ctrl1, CTRL1_CLK_FREQ_HI, div>>8);
BitHelper::Set(ctrl1, CTRL1_DATA_DTO, CTRL1_DATA_DTO_DEFAULT);
BitHelper::Set(ctrl1, CTRL1_CLK_INTLEN);
IoHelper::Write(emmc->CTRL1, ctrl1);
Task::Sleep(2);
if(IoHelper::Read(emmc->CTRL1, CTRL1_CLK_STABLE)==0)
	throw DeviceNotReadyException();
IoHelper::Write(emmc->CTRL1, CTRL1_CLK_EN, CTRL1_CLK_EN);
Task::Sleep(2);
}

VOID EmmcHost::SetRegister(UINT fn, UINT addr, UINT mask)
{
UINT value=ReadRegister(fn, addr);
value|=mask;
WriteRegister(fn, addr, value);
}

VOID EmmcHost::WriteRegister(UINT fn, UINT addr, BYTE value)
{
UINT arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, fn);
BitHelper::Set(arg, IO_RW_ADDR, addr);
BitHelper::Set(arg, IO_RW_DATA, value);
Command(CMD_IO_RW_DIRECT, arg);
}


//==========================
// Con-/Destructors Private
//==========================

EmmcHost::EmmcHost(SIZE_T addr, Irq irq):
m_Address(addr),
m_Irq(irq),
m_IrqFlags(0),
m_RelativeCardAddress(0)
{}


//================
// Common Private
//================

VOID EmmcHost::HandleInterrupt(VOID* param)
{
auto emmc=(EmmcHost*)param;
emmc->OnInterrupt();
}

VOID EmmcHost::OnInterrupt()
{
Status status=Status::Success;
auto emmc=(EMMC_REGS*)m_Address;
SpinLock lock(m_CriticalSection);
UINT irq_flags=IoHelper::Read(emmc->IRQ);
if(FlagHelper::Get(irq_flags, IRQF_ERR))
	status=Status::DeviceNotReady;
IoHelper::Write(emmc->IRQ, irq_flags);
if(FlagHelper::Get(m_IrqFlags, irq_flags))
	{
	m_IrqFlags=irq_flags;
	m_Signal.Trigger(status);
	}
}

}}