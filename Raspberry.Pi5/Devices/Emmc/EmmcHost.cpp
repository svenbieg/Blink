//==============
// EmmcHost.cpp
//==============

#include "EmmcHost.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/System/Interrupts.h"
#include "Devices/IoHelper.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Emmc {


//==========
// Settings
//==========

const UINT EMMC_TIMEOUT=100;


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

VOID EmmcHost::PowerOff()
{
SpinLock lock(m_CriticalSection);
Interrupts::SetHandler(m_Irq, nullptr);
IoHelper::Set(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
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


//==========================
// Con-/Destructors Private
//==========================

EmmcHost::EmmcHost(SIZE_T addr, Irq irq):
m_BlockSize(0),
m_Buffer(nullptr),
m_Device((EMMC_REGS*)addr),
m_Irq(irq),
m_RelativeCardAddress(0)
{
IoHelper::Write(m_Device->CTRL1, CTRL1_RESET_HOST);
Task::Sleep(10);
if(IoHelper::Read(m_Device->CTRL1, CTRL1_RESET_ALL))
	throw DeviceNotReadyException();
IoHelper::Set(m_Device->CTRL0, CTRL0_POWER, CTRL0_POWER_VDD1);
Task::Sleep(10);
Interrupts::SetHandler(m_Irq, this, &EmmcHost::HandleInterrupt);
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

VOID EmmcHost::HandleInterrupt()
{
SpinLock lock(m_CriticalSection);
UINT irq_flags=IoHelper::Read(m_Device->IRQ);
UINT irq_ack=irq_flags;
//BitHelper::Clear(irq_ack, IRQF_CARD);
IoHelper::Write(m_Device->IRQ, irq_ack);
UINT flags=irq_flags;
while(flags)
	{
	UINT lsb=Cpu::CountTrailingZeros(flags);
	UINT flag=1<<lsb;
	BitHelper::Clear(flags, flag);
	switch(flag)
		{
		case IRQF_CARD:
			{
			IoHelper::Clear(m_Device->IRQ_EN, IRQF_CARD);
			m_CardIrq.Trigger();
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

}}