//==============
// WifiSdio.cpp
//==============

#include "WifiSdio.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/Wifi/Wifi.h"
#include "Devices/Peripherals.h"

using namespace Concurrency;
using namespace Devices::Emmc;
using namespace Devices::Gpio;
using namespace Devices::Sdio;
using namespace Devices::Timers;

extern "C" UINT wifi_config;
extern "C" UINT wifi_config_size;
extern "C" UINT wifi_firmware;
extern "C" UINT wifi_firmware_size;
extern "C" UINT wifi_clm;
extern "C" UINT wifi_clm_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//======
// Emmc
//======

const SIZE_T EMMC_BASE				=AXI_EMMC1_BASE;

const UINT EMMC_BASE_CLOCK			=54'000'000;
const UINT EMMC_CLOCK				=25'000'000;

const UINT EMMC_BLOCK_SIZE[3]={ 64, 64, 512 };


//======
// Sdio
//======

const FN0_08 CCCR_IOENABLE			=0x0002;
const FN0_08 CCCR_IOREADY			=0x0003;
const FN0_08 CCCR_INT_EN			=0x0004;
const FN0_08 CCCR_INT_PENDING		=0x0005;
const FN0_08 CCCR_BUS_IFC			=0x0007;
const FN0_08 CCCR_BLKSIZE_FN0		=0x0010;
const FN0_08 CCCR_SPEED_CTRL		=0x0013;
const FN0_08 CCCR_BLKSIZE_FN1		=0x0110;
const FN0_08 CCCR_BLKSIZE_FN2_0		=0x0210;
const FN0_08 CCCR_BLKSIZE_FN2_1		=0x0211;

const UINT BUS_IFC_32BIT			=2;

const UINT SPEED_CTRL_EHS			=2;


//==================
// Con-/Destructors
//==================

WifiSdio::~WifiSdio()
{
GpioHelper::DigitalWrite(GpioArmPin::WifiOn, false);
m_ServiceTask->Cancel();
}


//==============
// Input-Stream
//==============

SIZE_T WifiSdio::Available()
{
throw NotImplementedException();
}

SIZE_T WifiSdio::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
UINT read=size;
while(read)
	{
	UINT copy=TypeHelper::Min(read, WIFI_PACKET_MAX);
	IoRwExtended(FN2, IO_RW_READ, 0, dst, copy);
	dst+=copy;
	read-=copy;
	}
return size;
}


//===============
// Output-Stream
//===============

VOID WifiSdio::Flush()
{
throw NotImplementedException();
}

SIZE_T WifiSdio::Write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE*)buf;
UINT write=size;
while(write)
	{
	UINT copy=TypeHelper::Min(write, WIFI_PACKET_MAX);
	IoRwExtended(FN2, IO_RW_WRITE, 0, src, copy);
	src+=copy;
	write-=copy;
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

WifiSdio::WifiSdio():
EmmcHost(AXI_EMMC1_BASE, Irq::Wifi),
m_Window(0)
{
m_ServiceTask=ServiceTask::Create(this, &WifiSdio::ServiceTask);
}


//================
// Common Private
//================

UINT WifiSdio::IoRwExtended(UINT fn, UINT flags, UINT addr, BYTE* buf, UINT size)
{
auto cmd_single=EmmcCmd::IoReadSingle;
auto cmd_multi=EmmcCmd::IoReadMulti;
if(BitHelper::Get(flags, IO_RW_WRITE))
	{
	cmd_single=EmmcCmd::IoWriteSingle;
	cmd_multi=EmmcCmd::IoWriteMulti;
	}
UINT response=0;
UINT block_size=EMMC_BLOCK_SIZE[fn];
while(size)
	{
	UINT arg=flags;
	BitHelper::Set(arg, IO_RW_FUNC, fn);
	BitHelper::Set(arg, IO_RW_ADDR, addr);
	UINT copy=size;
	UINT block_count=1;
	if(copy>=block_size)
		{
		block_count=TypeHelper::Min(copy/block_size, IO_RW_COUNT_MAX);
		copy=block_count*block_size;
		BitHelper::Set(arg, IO_RW_BLK|IO_RW_INCR);
		BitHelper::Set(arg, IO_RW_COUNT, block_count);
		Command(cmd_multi, arg, &response, buf, block_count, block_size);
		}
	else
		{
		BitHelper::Set(arg, IO_RW_COUNT, copy);
		Command(cmd_single, arg, &response, buf, 1, copy);
		}
	addr+=copy;
	buf+=copy;
	size-=copy;
	}
return response;
}

VOID WifiSdio::PollBackplane(UINT addr, UINT mask, UINT value, UINT timeout)
{
SetBackplane(addr);
addr&=SB_WND_MASK;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, FN1);
BitHelper::Set(arg, IO_RW_ADDR, SB_ADDR_32BIT|addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
UINT end=SystemTimer::GetTickCount64()+timeout;
do
	{
	UINT read=0;
	Command(EmmcCmd::IoReadSingle, arg, nullptr, &read, 1, sizeof(UINT));
	if(BitHelper::Get(read, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<=end);
throw TimeoutException();
}

VOID WifiSdio::PollRegister(SDIO_REG const& reg, BYTE mask, BYTE value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, reg.Function);
BitHelper::Set(arg, IO_RW_ADDR, reg.Address);
BitHelper::Set(arg, IO_RW_COUNT, 1);
do
	{
	UINT read=0;
	Command(EmmcCmd::IoRwDirect, arg, &read);
	if(BitHelper::Get((BYTE)read, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<end);
throw TimeoutException();
}

UINT WifiSdio::ReadBackplane(UINT addr)
{
SetBackplane(addr);
addr&=SB_WND_MASK;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, FN1);
BitHelper::Set(arg, IO_RW_ADDR, SB_ADDR_32BIT|addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
UINT value=0;
Command(EmmcCmd::IoReadSingle, arg, nullptr, &value, 1, sizeof(UINT));
return value;
}

VOID WifiSdio::ReadBackplane(UINT addr, UINT* buf, UINT size)
{
size=TypeHelper::AlignUp(size, sizeof(UINT));
auto dst=(BYTE*)buf;
while(size)
	{
	SetBackplane(addr);
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size, SB_WND_SIZE-wnd_pos);
	IoRwExtended(FN1, IO_RW_READ|IO_RW_INCR, SB_ADDR_32BIT|wnd_pos, dst, copy);
	addr+=copy;
	dst+=copy;
	size-=copy;
	}
}

BYTE WifiSdio::ReadRegister(SDIO_REG const& reg)
{
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, reg.Function);
BitHelper::Set(arg, IO_RW_ADDR, reg.Address);
BitHelper::Set(arg, IO_RW_COUNT, 1);
UINT value=0;
Command(EmmcCmd::IoRwDirect, arg, &value);
return (BYTE)value;
}

VOID WifiSdio::ResetDevice(UINT addr, UINT flags)
{
addr+=WIFI_WRAP;
UINT reset=RESET_BIT;
WriteBackplane(addr+COM_RESET, &reset, 1);
Task::Sleep(10);
UINT ioctrl=flags|IOCTRL_FGC|IOCTRL_CLOCK;
WriteBackplane(addr+COM_IOCTRL, &ioctrl, 1);
reset=0;
WriteBackplane(addr+COM_RESET, &reset, 1);
Task::Sleep(10);
ioctrl=flags|IOCTRL_CLOCK;
WriteBackplane(addr+COM_IOCTRL, &ioctrl, 1);
}

VOID WifiSdio::ServiceTask()
{
GpioHelper::SetPinMode(GpioArmPin::WifiOn, GpioArmPinMode::Output);
GpioHelper::DigitalWrite(GpioArmPin::WifiOn, true);
Task::Sleep(150);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioClk, GpioArmPinMode::Func4);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioCmd, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD0, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD1, GpioArmPinMode::Func3, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD2, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD3, GpioArmPinMode::Func3, GpioPullMode::PullUp);
SetClockRate(EMMC_BASE_CLOCK, EMMC_CLOCK);
Command(EmmcCmd::GoIdle);
UINT op_cond=Command(EmmcCmd::SendOpCond, 0);
for(UINT retry=0; retry<5; retry++)
	{
	if(FlagHelper::Get(op_cond, OP_COND_SUCCESS))
		break;
	Task::Sleep(10);
	op_cond=Command(EmmcCmd::SendOpCond, OP_COND_3V3);
	}
if(!FlagHelper::Get(op_cond, OP_COND_SUCCESS))
	throw DeviceNotReadyException();
UINT rel_addr=Command(EmmcCmd::SendRelAddr);
UINT rca=BitHelper::Get(rel_addr, RELADDR_RCA);
SelectCard(rca);
SetRegister(CCCR_SPEED_CTRL, SPEED_CTRL_EHS);
SetRegister(CCCR_BUS_IFC, BUS_IFC_32BIT);
IoHelper::Set(m_Device->CTRL0, CTRL0_HCTL_HS_EN|CTRL0_HCTL_DWIDTH4);
WriteRegister(CCCR_BLKSIZE_FN1, EMMC_BLOCK_SIZE[1]);
WriteRegister(CCCR_IOENABLE, FN1_BIT);
PollRegister(CCCR_IOREADY, FN1_BIT, FN1_BIT);
WriteRegister(SB_PULLUPS, 0);
ResetDevice(WIFI_ARM_BASE, ARM_IOCTRL_HALT);
WriteBackplane(ARM_RAM_BASE, &wifi_firmware, wifi_firmware_size);
WriteBackplane(ARM_RAM_BASE+ARM_RAM_SIZE-wifi_config_size, &wifi_config, wifi_config_size);
WriteBackplane(ARM_RESET_VECTOR, wifi_firmware);
ResetDevice(WIFI_ARM_BASE, 0);
PollRegister(SB_CLK_CSR, CLK_CSR_HT_AVAIL, CLK_CSR_HT_AVAIL);
WriteRegister(SB_CLK_CSR, CLK_CSR_HT_FORCE);
WriteBackplane(SDIO_MBOX_DATA_OUT, MBOX_DATA_VER);
WriteBackplane(SDIO_INT_MASK, INT_HOST);
WriteBackplane(SDIO_INT_STATUS, ~0U);
UINT block_size=EMMC_BLOCK_SIZE[FN2];
WriteRegister(CCCR_BLKSIZE_FN2_0, (BYTE)block_size);
WriteRegister(CCCR_BLKSIZE_FN2_1, (BYTE)(block_size>>8));
WriteRegister(CCCR_INT_EN, FN2_BIT|FN1_BIT|FN0_BIT);
SetRegister(CCCR_IOENABLE, FN2_BIT);
PollRegister(CCCR_IOREADY, FN2_BIT, FN2_BIT);
auto task=Task::Get();
SpinLock lock(m_CriticalSection);
IoHelper::Set(m_Device->IRQ_MASK, IRQF_CARD);
while(!task->Cancelled)
	{
	IoHelper::Set(m_Device->IRQ_EN, IRQF_CARD);
	m_CardIrq.Wait(lock);
	lock.Unlock();
	UINT pending=ReadRegister(CCCR_INT_PENDING);
	if(!pending)
		{
		lock.Lock();
		continue;
		}
	UINT int_status=ReadBackplane(SDIO_INT_STATUS);
	WriteBackplane(SDIO_INT_STATUS, int_status);
	UINT host_flags=BitHelper::Get(int_status, INT_HOST);
	while(host_flags)
		{
		UINT lsb=Cpu::CountTrailingZeros(host_flags);
		UINT host_flag=1U<<lsb;
		BitHelper::Clear(host_flags, host_flag);
		switch(host_flag)
			{
			case INT_FRAME:
				{
				auto pkt=WifiPacket::ReadFromStream(this);
				if(pkt)
					{
					PacketReceived(this, pkt);
					}
				else
					{
					throw DeviceNotReadyException();
					WriteRegister(SB_FRAME_CTRL, FRAME_CTRL_RFHALT);
					PollRegister(SB_RFRM_CNT_1, 0xFF, 0);
					PollRegister(SB_RFRM_CNT_0, 0xFF, 0);
					}
				break;
				}
			}
		}
	lock.Lock();
	}
}

VOID WifiSdio::SetBackplane(UINT addr)
{
addr&=~SB_WND_MASK;
if(m_Window==addr)
	return;
m_Window=addr;
WriteRegister(SB_WINDOW_0, (BYTE)(addr>>8));
WriteRegister(SB_WINDOW_1, (BYTE)(addr>>16));
WriteRegister(SB_WINDOW_2, (BYTE)(addr>>24));
}

VOID WifiSdio::SetRegister(SDIO_REG const& reg, BYTE mask, BYTE set)
{
BYTE value=ReadRegister(reg);
BitHelper::Clear(value, mask);
BitHelper::Set(value, set);
WriteRegister(reg, value);
}

VOID WifiSdio::WriteBackplane(UINT addr, UINT value)
{
SetBackplane(addr);
addr&=SB_WND_MASK;
UINT arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, FN1);
BitHelper::Set(arg, IO_RW_ADDR, SB_ADDR_32BIT|addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
Command(EmmcCmd::IoWriteSingle, arg, nullptr, &value, 1, sizeof(UINT));
}

VOID WifiSdio::WriteBackplane(UINT addr, UINT const* buf, UINT size)
{
size=TypeHelper::AlignUp(size, sizeof(UINT));
auto src=(BYTE*)buf;
while(size)
	{
	SetBackplane(addr);
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size, SB_WND_SIZE-wnd_pos);
	IoRwExtended(FN1, IO_RW_WRITE|IO_RW_INCR, SB_ADDR_32BIT|wnd_pos, src, copy);
	addr+=copy;
	src+=copy;
	size-=copy;
	}
}

VOID WifiSdio::WriteRegister(SDIO_REG const& reg, BYTE value)
{
UINT arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, reg.Function);
BitHelper::Set(arg, IO_RW_ADDR, reg.Address);
BitHelper::Set(arg, IO_RW_DATA, value);
Command(EmmcCmd::IoRwDirect, arg);
}

}}