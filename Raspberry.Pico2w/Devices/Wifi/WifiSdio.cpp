//==============
// WifiSdio.cpp
//==============

#include "WifiSdio.h"


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/Wifi/WifiAdapter.h"
#include "Devices/Peripherals.h"

using namespace Concurrency;
using namespace Devices::Gpio;
using namespace Devices::Sdio;
using namespace Devices::Timers;

extern UINT wifi_config;
extern UINT wifi_config_size;
extern UINT wifi_firmware;
extern UINT wifi_firmware_size;
extern UINT wifi_clm;
extern UINT wifi_clm_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==================
// Con-/Destructors
//==================

WifiSdio::~WifiSdio()
{
m_GpioHost->SetInterruptHandler(PIN_DATA, nullptr);
m_GpioHost->DigitalWrite(PIN_POWER, false);
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
assert(buf);
assert((SIZE_T)buf%sizeof(UINT)==0);
assert(size);
assert(size%sizeof(UINT)==0);
auto dst=(BYTE*)buf;
UINT cmd=CMD_INCR;
BitHelper::Set(cmd, CMD_FN, FN2);
UINT read=size;
while(read)
	{
	UINT copy=TypeHelper::Min(read, WIFI_BLOCK_SIZE);
	BitHelper::Set(cmd, CMD_SIZE, copy);
	SpiBegin(sizeof(UINT), copy);
	SpiWrite(&cmd, sizeof(UINT));
	SpiRead(dst, copy);
	SpiEnd();
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
assert(buf);
assert((SIZE_T)buf%sizeof(UINT)==0);
assert(size);
assert(size%sizeof(UINT)==0);
auto src=(BYTE const*)buf;
UINT cmd=CMD_WRITE|CMD_INCR;
BitHelper::Set(cmd, CMD_FN, FN2);
UINT write=size;
while(write)
	{
	UINT copy=TypeHelper::Min(write, WIFI_BLOCK_SIZE);
	BitHelper::Set(cmd, CMD_SIZE, copy);
	SpiBegin(sizeof(UINT)+copy, 0);
	SpiWrite(&cmd, sizeof(UINT));
	SpiWrite(src, copy);
	SpiEnd();
	src+=copy;
	write-=copy;
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

WifiSdio::WifiSdio():
m_Window(0)
{
m_ServiceTask=ServiceTask::Create(this, &WifiSdio::ServiceTask, "wifi_sdio", 1024);
}


//================
// Common Private
//================

VOID WifiSdio::HandleInterrupt()
{
m_GpioHost->DisableInterrupt(PIN_DATA);
m_InterruptPending.Trigger();
}

VOID WifiSdio::PollBackplane(UINT addr, UINT mask, UINT value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
do
	{
	UINT read=ReadBackplane(addr);
	if(BitHelper::Get(read, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<end);
throw TimeoutException();
}

VOID WifiSdio::PollRegister(SDIO_REG const& reg, UINT mask, UINT value, UINT timeout)
{
UINT64 end=SystemTimer::GetTickCount64()+timeout;
do
	{
	UINT read=ReadRegister(reg);
	if(BitHelper::Get(read, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<end);
throw TimeoutException();
}

UINT WifiSdio::Read16x2(FN0_32 const& reg)
{
UINT cmd=CMD_INCR;
BitHelper::Set(cmd, CMD_ADDR, reg.Address);
BitHelper::Set(cmd, CMD_SIZE, 4);
cmd=Swap16x2(cmd);
UINT value=0;
SpiBegin(sizeof(UINT), sizeof(UINT));
SpiWrite(&cmd, sizeof(UINT));
SpiRead(&value, sizeof(UINT));
SpiEnd();
value=Swap16x2(value);
return value;
}

UINT WifiSdio::ReadBackplane(UINT addr)
{
SetBackplane(addr);
addr&=SB_WND_MASK;
UINT cmd=CMD_INCR;
BitHelper::Set(cmd, CMD_FN, FN1);
BitHelper::Set(cmd, CMD_ADDR, SB_ADDR_32BIT|addr);
BitHelper::Set(cmd, CMD_SIZE, sizeof(UINT));
UINT delay_buf[SB_DELAY_COUNT];
SpiBegin(sizeof(UINT), SB_DELAY_BYTES+sizeof(UINT));
SpiWrite(&cmd, sizeof(UINT));
SpiRead(delay_buf, SB_DELAY_BYTES);
UINT value=0;
SpiRead(&value, sizeof(UINT));
SpiEnd();
return value;
}

VOID WifiSdio::ReadBackplane(UINT addr, VOID* buf, UINT size)
{
assert(buf);
assert((SIZE_T)buf%sizeof(UINT)==0);
auto dst=(BYTE*)buf;
UINT delay_buf[SB_DELAY_COUNT];
while(size)
	{
	SetBackplane(addr);
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size, SB_BLOCK_SIZE);
	UINT cmd=CMD_INCR;
	BitHelper::Set(cmd, CMD_FN, FN1);
	BitHelper::Set(cmd, CMD_ADDR, SB_ADDR_32BIT|wnd_pos);
	BitHelper::Set(cmd, CMD_SIZE, copy);
	UINT transfer=TypeHelper::AlignUp(copy, sizeof(UINT));
	SpiBegin(sizeof(UINT), SB_DELAY_BYTES+transfer);
	SpiWrite(&cmd, sizeof(UINT));
	SpiRead(delay_buf, SB_DELAY_BYTES);
	SpiRead(dst, transfer);
	SpiEnd();
	dst+=copy;
	addr+=copy;
	size-=copy;
	}
}

UINT WifiSdio::ReadRegister(SDIO_REG const& reg)
{
UINT cmd=CMD_INCR;
BitHelper::Set(cmd, CMD_FN, reg.Function);
BitHelper::Set(cmd, CMD_ADDR, reg.Address);
BitHelper::Set(cmd, CMD_SIZE, reg.Size);
UINT delay_count=0;
if(reg.Function==FN1)
	delay_count=SB_DELAY_COUNT;
SpiBegin(sizeof(UINT), (delay_count+1)*sizeof(UINT));
SpiWrite(&cmd, sizeof(UINT));
if(delay_count>0)
	{
	UINT delay_buf[SB_DELAY_COUNT];
	SpiRead(delay_buf, SB_DELAY_BYTES);
	}
UINT value=0;
SpiRead(&value, sizeof(UINT));
SpiEnd();
return value;
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
m_GpioHost->SetPinMode(PIN_POWER, GpioPinMode::Output);
m_GpioHost->DigitalWrite(PIN_POWER, true);
Task::Sleep(250);
for(UINT retry=0; retry>=0; retry++)
	{
	UINT test=Read16x2(SPI_TEST);
	if(test==0xFEEDBEAD)
		break;
	if(retry>3)
		throw DeviceNotReadyException();
	Task::Sleep(10);
	}
Write16x2(SPI_CTRL, SPI_CTRL_DEFAULT);
WriteRegister(SPI_INTE, INT_DEFAULT);
WriteRegister(SPI_RESP_DELAY_F1, SB_DELAY_BYTES);
WriteRegister(SB_PULLUPS, 0);
ResetDevice(WIFI_SRAM_BASE, 0);
WriteBackplane(SRAM_BANKX_INDEX, 3);
WriteBackplane(SRAM_BANKX_PDA, 0);
WriteBackplane(ARM_RAM_BASE, &wifi_firmware, wifi_firmware_size);
WriteBackplane(ARM_RAM_BASE+ARM_RAM_SIZE-wifi_config_size, &wifi_config, wifi_config_size);
ResetDevice(WIFI_ARM_BASE, 0);
PollRegister(SB_CLK_CSR, CLK_CSR_HT_AVAIL, CLK_CSR_HT_AVAIL);
WriteRegister(SB_CLK_CSR, CLK_CSR_HT_FORCE);
WriteBackplane(SDIO_MBOX_DATA_OUT, MBOX_DATA_VER);
WriteBackplane(SDIO_INT_MASK, INT_HOST);
WriteBackplane(SDIO_INT_STATUS, ~0U);
WriteRegister(SB_FN2_WATERMARK, FN2_SPI_WATERMARK);
PollRegister(SPI_STATUS, SPI_STATUS_F2_RX_READY, SPI_STATUS_F2_RX_READY);
m_GpioHost->SetPinMode(PIN_DATA, GpioPinMode::Input, GpioPullMode::PullDown);
m_GpioHost->SetInterruptHandler(PIN_DATA, this, &WifiSdio::HandleInterrupt, GpioIrqMode::None);
auto task=Task::Get();
SpinLock lock(m_CriticalSection);
while(!task->Cancelled)
	{
	m_GpioHost->EnableInterrupt(PIN_DATA, GpioIrqMode::High);
	m_InterruptPending.Wait(lock);
	lock.Unlock();
	WORD spi_ints=(WORD)ReadRegister(SPI_INTS);
	WriteRegister(SPI_INTS, spi_ints);
	UINT status=ReadRegister(SPI_STATUS);
	if(BitHelper::Get(status, SPI_STATUS_F2_PKT_AVAIL))
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

VOID WifiSdio::SetRegister(SDIO_REG const& reg, UINT mask, UINT set)
{
UINT value=ReadRegister(reg);
BitHelper::Clear(value, mask);
BitHelper::Set(value, set);
WriteRegister(reg, value);
}

VOID WifiSdio::Write16x2(FN0_32 const& reg, UINT value)
{
UINT cmd=CMD_WRITE|CMD_INCR;
BitHelper::Set(cmd, CMD_ADDR, reg.Address);
BitHelper::Set(cmd, CMD_SIZE, 4);
UINT buf[2];
buf[0]=Swap16x2(cmd);
buf[1]=Swap16x2(value);
SpiBegin(2*sizeof(UINT), 0);
SpiWrite(buf, 2*sizeof(UINT));
SpiEnd();
}

VOID WifiSdio::WriteBackplane(UINT addr, UINT value)
{
SetBackplane(addr);
addr&=SB_WND_MASK;
UINT cmd=CMD_WRITE|CMD_INCR;
BitHelper::Set(cmd, CMD_ADDR, SB_ADDR_32BIT|addr);
BitHelper::Set(cmd, CMD_SIZE, sizeof(UINT));
SpiBegin(2*sizeof(UINT), 0);
SpiWrite(&cmd, sizeof(UINT));
SpiWrite(&value, sizeof(UINT));
SpiEnd();
}

VOID WifiSdio::WriteBackplane(UINT addr, VOID const* buf, UINT size)
{
auto src=(BYTE const*)buf;
while(size)
	{
	SetBackplane(addr);
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size, SB_BLOCK_SIZE);
	UINT cmd=CMD_WRITE|CMD_INCR;
	BitHelper::Set(cmd, CMD_FN, FN1);
	BitHelper::Set(cmd, CMD_ADDR, SB_ADDR_32BIT|wnd_pos);
	BitHelper::Set(cmd, CMD_SIZE, copy);
	UINT transfer=TypeHelper::AlignUp(copy, sizeof(UINT));
	SpiBegin(sizeof(UINT)+transfer, 0);
	SpiWrite(&cmd, sizeof(UINT));
	SpiWrite(src, transfer);
	SpiEnd();
	src+=copy;
	addr+=copy;
	size-=copy;
	}
}

VOID WifiSdio::WriteRegister(SDIO_REG const& reg, UINT value)
{
UINT cmd=CMD_WRITE|CMD_INCR;
BitHelper::Set(cmd, CMD_FN, reg.Function);
BitHelper::Set(cmd, CMD_ADDR, reg.Address);
BitHelper::Set(cmd, CMD_SIZE, reg.Size);
UINT buf[2];
buf[0]=cmd;
buf[1]=value;
SpiBegin(2*sizeof(UINT), 0);
SpiWrite(buf, 2*sizeof(UINT));
SpiEnd();
}

}}