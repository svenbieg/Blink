//==============
// EmmcHost.cpp
//==============

#include "EmmcHost.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
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

VOID EmmcHost::Command(EmmcCmd cmd, UINT arg, UINT* response, VOID* buf, UINT block_count, UINT block_size)
{
SpinLock lock(m_CriticalSection);
Command(lock, cmd, arg, response, buf, block_count, block_size);
}

UINT EmmcHost::Command(EmmcAppCmd cmd, UINT arg)
{
UINT response[4];
Command(cmd, arg, response);
return response[0];
}

VOID EmmcHost::Command(EmmcAppCmd cmd, UINT arg, UINT* response, VOID* buf, UINT block_count, UINT block_size)
{
SpinLock lock(m_CriticalSection);
Command(lock, EmmcCmd::AppCmd, m_RelativeCardAddress<<16, nullptr, nullptr, 0, 0);
Command(lock, (EmmcCmd)cmd, arg, response, buf, block_size, block_count);
}

UINT EmmcHost::IoRwExtended(EMMC_FN const& fn, IoRwFlags flags, UINT addr, VOID* buf, UINT size)
{
auto cmd_single=EmmcCmd::IoReadSingle;
auto cmd_multi=EmmcCmd::IoReadMulti;
if(BitHelper::Get(flags, IO_RW_WRITE))
	{
	cmd_single=EmmcCmd::IoWriteSingle;
	cmd_multi=EmmcCmd::IoWriteMulti;
	}
UINT response=0;
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
	if(copy>=block_size)
		{
		block_count=TypeHelper::Min(copy/block_size, IO_RW_COUNT_MAX);
		copy=block_count*block_size;
		BitHelper::Set(arg, IO_RW_BLK|IO_RW_INCR);
		BitHelper::Set(arg, IO_RW_COUNT, block_count);
		Command(cmd_multi, arg, &response, ptr, block_count, block_size);
		}
	else
		{
		block_count=1;
		block_size=copy;
		BitHelper::Set(arg, IO_RW_COUNT, copy);
		Command(cmd_single, arg, &response, ptr, block_count, block_size);
		}
	addr+=copy;
	ptr+=copy;
	pos+=copy;
	}
return response;
}

VOID EmmcHost::PollRegister(EMMC_REG const& reg, BYTE mask, BYTE value, UINT timeout)
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
SpinLock lock(m_CriticalSection);
Interrupts::SetHandler(m_Irq, nullptr);
IoHelper::Set(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
}

BYTE EmmcHost::ReadRegister(EMMC_REG const& reg)
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
SpinLock lock(m_CriticalSection);
Command(lock, EmmcCmd::SelectCard, rca<<16, nullptr, nullptr, 0, 0);
m_RelativeCardAddress=rca;
}

VOID EmmcHost::SetClockRate(UINT base_clock, UINT clock_rate)
{
UINT ctrl1=IoHelper::Read(m_Device->CTRL1);
if(BitHelper::Get(ctrl1, CTRL1_CLK_EN))
	{
	BitHelper::Set(ctrl1, CTRL1_CLK_INTLEN|CTRL1_CLK_EN, 0U);
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
IoHelper::Set(m_Device->CTRL1, CTRL1_CLK_EN);
}

VOID EmmcHost::SetRegister(EMMC_REG const& reg, BYTE mask)
{
SetRegister(reg, mask, mask);
}

VOID EmmcHost::SetRegister(EMMC_REG const& reg, BYTE mask, BYTE value)
{
SpinLock lock(m_CriticalSection);
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, reg.FN.ID);
BitHelper::Set(arg, IO_RW_ADDR, reg.ADDR);
UINT tmp=0;
Command(lock, EmmcCmd::IoRwDirect, arg, &tmp);
tmp&=~mask;
tmp|=value;
arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, reg.FN.ID);
BitHelper::Set(arg, IO_RW_ADDR, reg.ADDR);
BitHelper::Set(arg, IO_RW_DATA, (BYTE)tmp);
Command(lock, EmmcCmd::IoRwDirect, arg);
}

VOID EmmcHost::WriteRegister(EMMC_REG const& reg, BYTE value)
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
m_BlockSize(0),
m_Buffer(nullptr),
m_Device((EMMC_REGS*)addr),
m_Irq(irq),
m_IrqFlags(0),
m_RelativeCardAddress(0)
{
IoHelper::Write(m_Device->CTRL1, CTRL1_RESET_HOST);
Task::Sleep(10);
if(IoHelper::Read(m_Device->CTRL1, CTRL1_RESET_ALL))
	throw DeviceNotReadyException();
IoHelper::Set(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_VDD1);
Task::Sleep(10);
m_ServiceTask=ServiceTask::Create(this, &EmmcHost::ServiceTask, "emmc");
Interrupts::SetHandler(m_Irq, HandleInterrupt, this);
IoHelper::Write(m_Device->IRQ_MASK, IRQF_DEFAULT);
IoHelper::Write(m_Device->IRQ_EN, IRQF_DEFAULT);
IoHelper::Write(m_Device->IRQ, ~0U);
}


//================
// Common Private
//================

VOID EmmcHost::Command(SpinLock& lock, EmmcCmd cmd, UINT arg, UINT* response, VOID* buf, UINT block_count, UINT block_size)
{
assert(block_count<0x10000);
assert((SIZE_T)buf%sizeof(UINT)==0);
if(IoHelper::Read(m_Device->STATUS, STATUS_CMD_INHIBIT|STATUS_DAT_INHIBIT))
	throw DeviceNotReadyException();
m_Buffer=(UINT*)buf;
m_BlockSize=block_size;
IoHelper::Write(m_Device->BLKSIZECNT, (block_count<<16)|block_size);
IoHelper::Write(m_Device->ARG1, arg);
IoHelper::Write(m_Device->CMD, (UINT)cmd);
m_CommandDone.Wait(lock, EMMC_TIMEOUT);
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
if(m_Buffer)
	m_DataDone.Wait(lock, EMMC_TIMEOUT);
}

VOID EmmcHost::HandleInterrupt(VOID* param)
{
auto emmc=(EmmcHost*)param;
emmc->OnInterrupt();
}

VOID EmmcHost::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
m_IrqFlags=IoHelper::Read(m_Device->IRQ);
Status status=Status::Success;
if(FlagHelper::Get(m_IrqFlags, IRQF_ERR))
	status=Status::DeviceNotReady;
m_IrqPending.Trigger(status);
}

VOID EmmcHost::ServiceTask()
{
auto task=Task::Get();
SpinLock lock(m_CriticalSection);
while(!task->Cancelled)
	{
	m_IrqPending.Wait(lock);
	UINT irq_flags=m_IrqFlags;
	UINT irq_ack=m_IrqFlags;
	BitHelper::Clear(irq_ack, IRQF_CARD);
	IoHelper::Write(m_Device->IRQ, irq_ack);
	while(irq_flags)
		{
		UINT lsb=Cpu::GetLeastSignificantBitPosition(irq_flags);
		UINT irq_flag=1<<lsb;
		BitHelper::Clear(irq_flags, irq_flag);
		switch(irq_flag)
			{
			case IRQF_CARD:
				{
				if(m_IrqFlags!=IRQF_CARD)
					break;
				CardIrq(this);
				break;
				}
			case IRQF_CMD_DONE:
				{
				m_CommandDone.Trigger();
				break;
				}
			case IRQF_DATA_DONE:
				{
				m_Buffer=nullptr;
				m_DataDone.Trigger();
				break;
				}
			case IRQF_WRITE_RDY:
				{
				assert(m_Buffer);
				auto end=&m_Buffer[m_BlockSize/sizeof(UINT)];
				while(m_Buffer<end)
					m_Device->DATA=*m_Buffer++;
				break;
				}
			case IRQF_READ_RDY:
				{
				assert(m_Buffer);
				auto end=&m_Buffer[m_BlockSize/sizeof(UINT)];
				while(m_Buffer<end)
					*m_Buffer++=m_Device->DATA;
				break;
				}
			}
		}
	}
}

}}