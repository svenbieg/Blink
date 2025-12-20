//==============
// EmmcHost.cpp
//==============

#include "Devices/Emmc/EmmcHost.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"

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

constexpr UINT BLOCK_SIZE=512;


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

VOID EmmcHost::AppCommand(UINT cmd, UINT arg, UINT* response, UINT* buf, UINT block_size, UINT block_count, UINT timeout)
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

VOID EmmcHost::Command(UINT cmd, UINT arg, UINT* response, UINT* buf, UINT block_size, UINT block_count, UINT timeout)
{
assert(block_count<0x10000);
auto emmc=(emmc_regs_t*)m_Address;
SpinLock lock(m_CriticalSection);
if(io_read(emmc->STATUS, STATUS_CMD_INHIBIT))
	throw DeviceNotReadyException();
io_write(emmc->BLKSIZECNT, (block_count<<16)|block_size);
io_write(emmc->ARG1, arg);
io_write(emmc->CMD, cmd);
m_IrqFlags=IRQF_CMD_DONE|IRQF_ERR;
m_Signal.Wait(lock, timeout);
if(FlagHelper::Get(m_IrqFlags, IRQF_ERR))
	throw DeviceNotReadyException();
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
if(FlagHelper::Get(cmd, CMF_ISDATA))
	{
	if(FlagHelper::Get(cmd, CMF_DAT_DIR_READ))
		{
		for(UINT block=0; block<block_count; block++)
			{
			m_IrqFlags=IRQF_READ_RDY|IRQF_ERR;
			m_Signal.Wait(lock, timeout);
			if(FlagHelper::Get(m_IrqFlags, IRQF_ERR))
				throw DeviceNotReadyException();
			for(UINT pos=0; pos<block_size; pos+=sizeof(UINT))
				*buf++=emmc->DATA;
			}
		}
	else
		{
		for(UINT block=0; block<block_count; block++)
			{
			if(!FlagHelper::Get(m_IrqFlags, IRQF_WRITE_RDY))
				{
				m_IrqFlags=IRQF_WRITE_RDY|IRQF_ERR;
				m_Signal.Wait(lock, timeout);
				if(FlagHelper::Get(m_IrqFlags, IRQF_ERR))
					throw DeviceNotReadyException();
				}
			for(UINT pos=0; pos<block_size; pos+=sizeof(UINT))
				emmc->DATA=*buf++;
			}
		}
	}
}

FILE_SIZE EmmcHost::GetCapacity()
{
auto emmc=(emmc_regs_t*)m_Address;
UINT csd[4];
Command(CMD_SEND_CSD, m_RelativeCardAddress<<16, csd);
UINT csd_ver=csd[3]>>30;
UINT size=(((csd[2]&0x3FF)<<2)|csd[1]>>30)+1;
UINT shift=((csd[1]>>15)&0x7)+12;
FILE_SIZE capacity=((FILE_SIZE)size<<shift)*m_BlockSize;
return capacity;
}

VOID EmmcHost::PowerOff()
{
Interrupts::SetHandler(m_Irq, nullptr);
auto emmc=(emmc_regs_t*)m_Address;
io_write(emmc->CTRL0, CTRL0_POWER, CTRL0_POWER_OFF);
}

VOID EmmcHost::Read(UINT block, UINT block_count, UINT block_size, UINT* buf)
{
UINT cmd=block_count>1? CMD_READ_MULTI: CMD_READ_SINGLE;
Command(cmd, block, nullptr, buf, block_size, block_count, 500);
}

UINT EmmcHost::ReadRegister(UINT fn, UINT addr)
{
UINT cmd=IO_RW_READ;
cmd|=(fn&7)<<28;
cmd|=(addr&0x1FFFF)<<9;
return Command(CMD_IO_RW_DIRECT, cmd);
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
//UINT sel_card=Command(CMD_SELECT_CARD, rca<<16);
//UINT sel_status=(sel_card>>9)&0xF;
//assert(sel_status==3||sel_status==4);
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

VOID EmmcHost::Write(UINT block, UINT block_count, UINT block_size, UINT const* buf)
{
UINT cmd=block_count>1? CMD_WRITE_MULTI: CMD_WRITE_SINGLE;
Command(cmd, block, nullptr, (UINT*)buf, block_size, block_count, 500);
}

VOID EmmcHost::WriteRegister(UINT fn, UINT addr, UINT value)
{
UINT cmd=IO_RW_WRITE;
cmd|=(fn&7)<<28;
cmd|=(addr&0x1FFFF)<<9;
cmd|=(value&0xFF);
Command(CMD_IO_RW_DIRECT, cmd);
}


//==========================
// Con-/Destructors Private
//==========================

EmmcHost::EmmcHost(SIZE_T addr, Irq irq):
m_Address(addr),
m_BlockSize(BLOCK_SIZE),
m_Irq(irq),
m_IrqFlags(0),
m_RelativeCardAddress(0)
{}


//================
// Common Private
//================

VOID EmmcHost::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
auto emmc=(emmc_regs_t*)m_Address;
UINT irq_flags=io_read(emmc->IRQ);
io_write(emmc->IRQ, UINT_MAX);
if(m_IrqFlags&irq_flags)
	{
	m_IrqFlags=irq_flags;
	m_Signal.Trigger();
	}
}

VOID EmmcHost::HandleInterrupt(VOID* param)
{
auto emmc=(EmmcHost*)param;
emmc->OnInterrupt();
}

}}