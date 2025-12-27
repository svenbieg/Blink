//==============
// EmmcHost.cpp
//==============

#include "EmmcHost.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "Devices/Timers/SystemTimer.h"

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
auto emmc=(emmc_regs_t*)m_Address;
SpinLock lock(m_CriticalSection);
if(io_read(emmc->STATUS, STATUS_CMD_INHIBIT))
	throw DeviceNotReadyException();
if(io_read(emmc->STATUS, STATUS_DAT_INHIBIT))
	{
	io_set(emmc->CTRL1, CTRL1_RESET_DATA);
	for(UINT retry=0; retry>=0; retry++)
		{
		if(io_read(emmc->STATUS, STATUS_DAT_INHIBIT)==0)
			break;
		if(retry==10)
			throw DeviceNotReadyException();
		}
	}
io_write(emmc->BLKSIZECNT, (block_count<<16)|block_size);
io_write(emmc->ARG1, arg);
io_write(emmc->CMD, cmd);
m_IrqFlags=IRQF_CMD_DONE|IRQF_ERR;
m_Signal.Wait(lock, timeout);
uint32_t resp_type=bits_get(cmd, CMF_RSPNS_MASK);
if(response)
	{
	switch(resp_type)
		{
		case CMF_RSPNS_136:
			{
			response[0]=io_read(emmc->RESP[0]);
			response[1]=io_read(emmc->RESP[1]);
			response[2]=io_read(emmc->RESP[2]);
			response[3]=io_read(emmc->RESP[3]);
			break;
			}
		case CMF_RSPNS_48:
		case CMF_RSPNS_48_BUSY:
			{
			response[0]=io_read(emmc->RESP[0]);
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
		if(pos==size)
			break;
		}
	if(!FlagHelper::Get(m_IrqFlags, IRQF_DATA_DONE))
		{
		m_IrqFlags=IRQF_DATA_DONE|IRQF_ERR;
		m_Signal.Wait(lock, timeout);
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
		if(pos==size)
			break;
		}
	}
}

VOID EmmcHost::PollRegister(UINT fn, UINT addr, BYTE mask, BYTE value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
UINT arg=IO_RW_READ;
bits_set(arg, IO_RW_FUNC, fn);
bits_set(arg, IO_RW_ADDR, addr);
UINT response=0;
do
	{
	Command(CMD_IO_RW_DIRECT, arg, &response);
	if(bits_get((BYTE)response, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<=end);
throw TimeoutException();
}

VOID EmmcHost::PowerOff()
{
Interrupts::SetHandler(m_Irq, nullptr);
auto emmc=(emmc_regs_t*)m_Address;
io_write(emmc->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
}

BYTE EmmcHost::ReadRegister(UINT fn, UINT addr)
{
UINT arg=IO_RW_READ;
bits_set(arg, IO_RW_FUNC, fn);
bits_set(arg, IO_RW_ADDR, addr);
UINT value=0;
Command(CMD_IO_RW_DIRECT, arg, &value);
return (BYTE)value;
}

VOID EmmcHost::Reset()
{
Interrupts::SetHandler(m_Irq, nullptr);
auto emmc=(emmc_regs_t*)m_Address;
io_write(emmc->CTRL1, CTRL1_RESET_HOST);
Task::Sleep(10);
if(io_read(emmc->CTRL1, CTRL1_RESET_ALL))
	throw DeviceNotReadyException();
io_write(emmc->CTRL0, CTRL0_POWER, CTRL0_POWER_VDD1);
Task::Sleep(2);
Interrupts::SetHandler(m_Irq, HandleInterrupt, this);
io_write(emmc->IRQ, UINT_MAX);
io_write(emmc->IRQ_MASK, IRQF_MASK_DEFAULT);
io_write(emmc->IRQ_EN, IRQF_MASK_DEFAULT);
Task::Sleep(2);
}

VOID EmmcHost::SelectCard(UINT rca)
{
m_RelativeCardAddress=rca;
Command(CMD_SELECT_CARD, rca<<16);
}

VOID EmmcHost::SetClockRate(UINT base_clock, UINT clock_rate)
{
auto emmc=(emmc_regs_t*)m_Address;
uint32_t ctrl1=io_read(emmc->CTRL1);
if(bits_get(ctrl1, CTRL1_CLK_EN))
	{
	bits_set(ctrl1, CTRL1_CLK_INTLEN|CTRL1_CLK_EN, 0);
	io_write(emmc->CTRL1, ctrl1);
	Task::Sleep(2);
	}
uint32_t div=base_clock/(clock_rate<<1);
bits_set(ctrl1, CTRL1_CLK_FREQ_LO, div);
bits_set(ctrl1, CTRL1_CLK_FREQ_HI, div>>8);
bits_set(ctrl1, CTRL1_DATA_DTO, CTRL1_DATA_DTO_DEFAULT);
bits_set(ctrl1, CTRL1_CLK_INTLEN);
io_write(emmc->CTRL1, ctrl1);
Task::Sleep(2);
if(io_read(emmc->CTRL1, CTRL1_CLK_STABLE)==0)
	throw DeviceNotReadyException();
io_write(emmc->CTRL1, CTRL1_CLK_EN, CTRL1_CLK_EN);
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
bits_set(arg, IO_RW_FUNC, fn);
bits_set(arg, IO_RW_ADDR, addr);
bits_set(arg, IO_RW_DATA, value);
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
auto emmc=(emmc_regs_t*)m_Address;
SpinLock lock(m_CriticalSection);
UINT irq_flags=io_read(emmc->IRQ);
if(FlagHelper::Get(irq_flags, IRQF_ERR))
	status=Status::DeviceNotReady;
io_write(emmc->IRQ, irq_flags);
if(FlagHelper::Get(m_IrqFlags, irq_flags))
	{
	m_IrqFlags=irq_flags;
	m_Signal.Trigger(status);
	}
}

}}