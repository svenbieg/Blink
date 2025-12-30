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


//==========
// Settings
//==========

constexpr UINT EMMC_TIMEOUT=100;


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

UINT EmmcHost::Command(EmmcCmd cmd, UINT arg)
{
UINT response[4];
Command(cmd, arg, response, nullptr, 0, 0);
return response[0];
}

VOID EmmcHost::Command(EmmcCmd cmd, UINT arg, UINT* response)
{
Command(cmd, arg, response, nullptr, 0, 0);
}

VOID EmmcHost::Command(EmmcCmd cmd, UINT arg, UINT* response, VOID* buf, UINT block_count, UINT block_size)
{
assert(block_count<0x10000);
SpinLock lock(m_CriticalSection);
if(IoHelper::Read(m_Device->STATUS, STATUS_CMD_INHIBIT))
	throw DeviceNotReadyException();
if(IoHelper::Read(m_Device->STATUS, STATUS_DAT_INHIBIT))
	{
	IoHelper::Set(m_Device->CTRL1, CTRL1_RESET_DATA);
	for(UINT retry=0; retry>=0; retry++)
		{
		if(IoHelper::Read(m_Device->STATUS, STATUS_DAT_INHIBIT)==0)
			break;
		if(retry==10)
			throw DeviceNotReadyException();
		}
	}
IoHelper::Write(m_Device->BLKSIZECNT, (block_count<<16)|block_size);
IoHelper::Write(m_Device->ARG1, arg);
IoHelper::Write(m_Device->CMD, (UINT)cmd);
m_IrqFlags=IRQF_CMD_DONE|IRQF_ERR;
m_Signal.Wait(lock, EMMC_TIMEOUT);
UINT resp_type=BitHelper::Get(cmd, CMF_RSPNS_MASK);
if(response)
	{
	switch(resp_type)
		{
		case CMF_RSPNS_136:
			{
			response[0]=IoHelper::Read(m_Device->RESP[0]);
			response[1]=IoHelper::Read(m_Device->RESP[1]);
			response[2]=IoHelper::Read(m_Device->RESP[2]);
			response[3]=IoHelper::Read(m_Device->RESP[3]);
			break;
			}
		case CMF_RSPNS_48:
		case CMF_RSPNS_48_BUSY:
			{
			response[0]=IoHelper::Read(m_Device->RESP[0]);
			break;
			}
		default:
			break;
		}
	}
if(!FlagHelper::Get(cmd, CMF_ISDATA))
	return;
assert((SIZE_T)buf%sizeof(UINT)==0);
UINT size=block_count*block_size;
UINT pos=0;
if(FlagHelper::Get(cmd, CMF_DAT_DIR_READ))
	{
	auto dst=(UINT*)buf;
	for(UINT block=0; block<block_count; block++)
		{
		if(!FlagHelper::Get(m_Device->STATUS, STATUS_READ_AVAILABLE))
			{
			m_IrqFlags=IRQF_READ_RDY|IRQF_ERR;
			m_Signal.Wait(lock, EMMC_TIMEOUT);
			}
		UINT copy=TypeHelper::Min(size-pos, block_size);
		auto end=dst+(copy/sizeof(UINT));
		while(dst<end)
			*dst++=m_Device->DATA;
		pos+=copy;
		}
	}
else
	{
	auto src=(UINT*)buf;
	for(UINT block=0; block<block_count; block++)
		{
		if(!FlagHelper::Get(m_Device->STATUS, STATUS_WRITE_AVAILABLE))
			{
			m_IrqFlags=IRQF_WRITE_RDY|IRQF_ERR;
			m_Signal.Wait(lock, EMMC_TIMEOUT);
			}
		UINT copy=TypeHelper::Min(size-pos, block_size);
		auto end=src+(copy/sizeof(UINT));
		while(src<end)
			m_Device->DATA=*src++;
		pos+=copy;
		}
	}
if(!FlagHelper::Get(m_IrqFlags, IRQF_DATA_DONE))
	{
	m_IrqFlags=IRQF_DATA_DONE|IRQF_ERR;
	m_Signal.Wait(lock, EMMC_TIMEOUT);
	}
}

UINT EmmcHost::Command(EmmcAppCmd cmd, UINT arg)
{
UINT response[4];
Command(EmmcCmd::AppCmd, m_RelativeCardAddress<<16);
Command((EmmcCmd)cmd, arg, response, nullptr, 0, 0);
return response[0];
}

VOID EmmcHost::Command(EmmcAppCmd cmd, UINT arg, UINT* response)
{
Command(EmmcCmd::AppCmd, m_RelativeCardAddress<<16);
Command((EmmcCmd)cmd, arg, response, nullptr, 0, 0);
}

VOID EmmcHost::Command(EmmcAppCmd cmd, UINT arg, UINT* response, VOID* buf, UINT block_count, UINT block_size)
{
Command(EmmcCmd::AppCmd, m_RelativeCardAddress<<16);
Command((EmmcCmd)cmd, arg, response, buf, block_size, block_count);
}

VOID EmmcHost::IoRwExtended(EMMC_FN fn, IoRwFlags flags, UINT addr, VOID* buf, UINT size)
{
assert(size<=IO_RW_ADDR_MAX);
auto cmd_single=EmmcCmd::IoReadSingle;
auto cmd_multi=EmmcCmd::IoReadMulti;
if(BitHelper::Get(flags, IO_RW_WRITE))
	{
	cmd_single=EmmcCmd::IoWriteSingle;
	cmd_multi=EmmcCmd::IoWriteMulti;
	}
auto ptr=(BYTE*)buf;
UINT block_size=fn.BLOCK_SIZE;
UINT pos=0;
while(pos<size)
	{
	UINT arg=(UINT)flags;
	BitHelper::Set(arg, IO_RW_FUNC, fn.ID);
	BitHelper::Set(arg, IO_RW_ADDR, addr);
	UINT copy=size-pos;
	UINT block_count=1;
	if(copy>block_size)
		{
		block_count=TypeHelper::Min(copy/block_size, IO_RW_COUNT_MAX);
		copy=block_count*block_size;
		BitHelper::Set(arg, IO_RW_BLK);
		BitHelper::Set(arg, IO_RW_COUNT, block_count);
		Command(cmd_multi, arg, nullptr, ptr, block_count, block_size);
		}
	else
		{
		block_count=1;
		block_size=copy;
		BitHelper::Set(arg, IO_RW_COUNT, copy);
		Command(cmd_single, arg, nullptr, ptr, block_count, block_size);
		}
	addr+=copy;
	ptr+=copy;
	pos+=copy;
	}
}

VOID EmmcHost::PollRegister(EMMC_REG reg, BYTE mask, BYTE value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, reg.FN.ID);
BitHelper::Set(arg, IO_RW_ADDR, reg.ADDR);
UINT response=0;
do
	{
	Command(EmmcCmd::IoRwDirect, arg, &response);
	if(BitHelper::Get((BYTE)response, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<=end);
throw TimeoutException();
}

VOID EmmcHost::PowerOff()
{
Interrupts::SetHandler(m_Irq, nullptr);
IoHelper::Write(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
}

BYTE EmmcHost::ReadRegister(EMMC_REG reg)
{
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, reg.FN.ID);
BitHelper::Set(arg, IO_RW_ADDR, reg.ADDR);
UINT value=0;
Command(EmmcCmd::IoRwDirect, arg, &value);
return (BYTE)value;
}

VOID EmmcHost::SelectCard(UINT rca)
{
m_RelativeCardAddress=rca;
Command(EmmcCmd::SelectCard, rca<<16);
}

VOID EmmcHost::SetClockRate(UINT base_clock, UINT clock_rate)
{
UINT ctrl1=IoHelper::Read(m_Device->CTRL1);
if(BitHelper::Get(ctrl1, CTRL1_CLK_EN))
	{
	BitHelper::Set(ctrl1, CTRL1_CLK_INTLEN|CTRL1_CLK_EN, 0);
	IoHelper::Write(m_Device->CTRL1, ctrl1);
	Task::Sleep(10);
	}
UINT div=base_clock/(clock_rate<<1);
BitHelper::Set(ctrl1, CTRL1_CLK_FREQ_LO, div);
BitHelper::Set(ctrl1, CTRL1_CLK_FREQ_HI, div>>8);
BitHelper::Set(ctrl1, CTRL1_DATA_DTO, CTRL1_DATA_DTO_DEFAULT);
BitHelper::Set(ctrl1, CTRL1_CLK_INTLEN);
IoHelper::Write(m_Device->CTRL1, ctrl1);
Task::Sleep(10);
if(IoHelper::Read(m_Device->CTRL1, CTRL1_CLK_STABLE)==0)
	throw DeviceNotReadyException();
IoHelper::Write(m_Device->CTRL1, CTRL1_CLK_EN, CTRL1_CLK_EN);
Task::Sleep(10);
}

VOID EmmcHost::SetRegister(EMMC_REG reg, BYTE mask)
{
SetRegister(reg, mask, mask);
}

VOID EmmcHost::SetRegister(EMMC_REG reg, BYTE mask, BYTE value)
{
UINT tmp=ReadRegister(reg);
tmp&=~mask;
tmp|=value;
WriteRegister(reg, tmp);
}

VOID EmmcHost::WriteRegister(EMMC_REG reg, BYTE value)
{
UINT arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, reg.FN.ID);
BitHelper::Set(arg, IO_RW_ADDR, reg.ADDR);
BitHelper::Set(arg, IO_RW_DATA, value);
Command(EmmcCmd::IoRwDirect, arg);
}


//==========================
// Con-/Destructors Private
//==========================

EmmcHost::EmmcHost(SIZE_T addr, Irq irq):
m_Device((EMMC_REGS*)addr),
m_Irq(irq),
m_IrqFlags(0),
m_RelativeCardAddress(0)
{
Interrupts::SetHandler(m_Irq, nullptr);
IoHelper::Write(m_Device->CTRL1, CTRL1_RESET_HOST);
Task::Sleep(10);
if(IoHelper::Read(m_Device->CTRL1, CTRL1_RESET_ALL))
	throw DeviceNotReadyException();
IoHelper::Write(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_VDD1);
Task::Sleep(10);
Interrupts::SetHandler(m_Irq, HandleInterrupt, this);
IoHelper::Write(m_Device->IRQ, UINT_MAX);
IoHelper::Write(m_Device->IRQ_MASK, IRQF_MASK_DEFAULT);
IoHelper::Write(m_Device->IRQ_EN, IRQF_MASK_DEFAULT);
Task::Sleep(10);
}


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
SpinLock lock(m_CriticalSection);
UINT irq_flags=IoHelper::Read(m_Device->IRQ);
if(FlagHelper::Get(irq_flags, IRQF_ERR))
	status=Status::DeviceNotReady;
IoHelper::Write(m_Device->IRQ, irq_flags);
if(FlagHelper::Get(m_IrqFlags, irq_flags))
	{
	m_IrqFlags=irq_flags;
	m_Signal.Trigger(status);
	}
}

}}