//=================
// WifiAdapter.cpp
//=================

#include "WifiAdapter.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"
#include "Devices/Gpio/GpioHelper.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/Wifi/WifiEvents.h"
#include "Storage/Buffer.h"
#include <base.h>

using namespace Concurrency;
using namespace Devices::Emmc;
using namespace Devices::Gpio;
using namespace Devices::System;
using namespace Devices::Timers;
using namespace Network::Ethernet;
using namespace Storage;

extern BYTE wifi_clm[];
extern UINT wifi_clm_size;
extern BYTE wifi_config[];
extern UINT wifi_config_size;
extern BYTE wifi_firmware[];
extern UINT wifi_firmware_size;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==========
// Settings
//==========

constexpr SIZE_T EMMC_BASE					=AXI_EMMC1_BASE;
constexpr UINT EMMC_BASE_CLOCK				=54'000'000;
constexpr UINT EMMC_CLOCK					=25'000'000;


//======
// Emmc
//======

constexpr EMMC_FN FN0						={ 0, 64 };

constexpr EMMC_REG CCCR_IOENABLE			={ FN0, 0x02 };
constexpr EMMC_REG CCCR_IOREADY				={ FN0, 0x03 };
constexpr EMMC_REG CCCR_INT_EN				={ FN0, 0x04 };
constexpr EMMC_REG CCCR_INT_PENDING			={ FN0, 0x05 };
constexpr EMMC_REG CCCR_BUS_IFC				={ FN0, 0x07 };
constexpr EMMC_REG CCCR_BLKSIZE_FN0			={ FN0, 0x10 };
constexpr EMMC_REG CCCR_SPEED_CTRL			={ FN0, 0x13 };
constexpr EMMC_REG CCCR_BLKSIZE_FN1			={ FN0, 0x110 };
constexpr EMMC_REG CCCR_BLKSIZE_FN2_0		={ FN0, 0x210 };
constexpr EMMC_REG CCCR_BLKSIZE_FN2_1		={ FN0, 0x211 };

constexpr UINT BUS_IFC_32BIT				=2;

constexpr UINT FN0_BIT						=(1<<0);
constexpr UINT FN1_BIT						=(1<<1);
constexpr UINT FN2_BIT						=(1<<2);

constexpr UINT SPEED_CTRL_EHS				=2;


//===========
// Backplane
//===========

constexpr EMMC_FN FN1						={ 1, 64 };

constexpr EMMC_REG SB_WINDOW_0				={ FN1, 0x1000A };
constexpr EMMC_REG SB_WINDOW_1				={ FN1, 0x1000B };
constexpr EMMC_REG SB_WINDOW_2				={ FN1, 0x1000C };
constexpr EMMC_REG SB_FRAME_CTRL			={ FN1, 0x1000D };
constexpr EMMC_REG SB_CLK_CSR				={ FN1, 0x1000E };
constexpr EMMC_REG SB_PULLUPS				={ FN1, 0x1000F };
constexpr EMMC_REG SB_WFRM_CNT				={ FN1, 0x10019 };
constexpr EMMC_REG SB_RFRM_CNT_0			={ FN1, 0x1001B };
constexpr EMMC_REG SB_RFRM_CNT_1			={ FN1, 0x1001C };

constexpr BYTE CLK_CSR_HT_AVAIL				=(1<<7);
constexpr BYTE CLK_CSR_FORCE_HT				=(1<<1);

constexpr BYTE FRAME_CTRL_RFHALT			=(1<<0);
constexpr BYTE FRAME_CTRL_WFHALT			=(1<<1);

constexpr UINT SB_ADDR_32BIT				=(1<<15);
constexpr UINT SB_WND_SIZE					=0x8000;
constexpr UINT SB_WND_MASK					=0x7FFF;


//========
// Common
//========

constexpr UINT COM_BASE					=0x18000000;

constexpr UINT IOCTRL					=0x408;
constexpr UINT IOCTRL_FGC				=(1<<1);
constexpr UINT IOCTRL_CLOCK				=(1<<0);
constexpr UINT RESET					=0x800;
constexpr UINT RESET_BIT				=(1<<0);


//=====
// Arm
//=====

constexpr UINT ARM_BASE					=0x18102000;
constexpr UINT ARM_IOCTRL_HALT			=(1<<5);
constexpr UINT ARM_RAM_BASE				=0x198000;
constexpr UINT ARM_RAM_SIZE				=0xC8000;
constexpr UINT ARM_RESET_VECTOR			=0;


//======
// Sdio
//======

constexpr UINT SDIO_BASE				=0x18004000;
constexpr UINT SDIO_INT_STATUS			=SDIO_BASE+0x20;
constexpr UINT SDIO_INT_MASK			=SDIO_BASE+0x24;
constexpr UINT SDIO_MBOX				=SDIO_BASE+0x40;
constexpr UINT SDIO_MBOX_DATA_OUT		=SDIO_BASE+0x48;
constexpr UINT SDIO_MBOX_DATA_IN		=SDIO_BASE+0x4C;

constexpr UINT INT_MAILBOX				=(1<<7);
constexpr UINT INT_FRAME				=(1<<6);
constexpr UINT INT_FCCHANGE				=(1<<5);
constexpr UINT INT_FCSTATE				=(1<<4);
constexpr UINT INT_HOST					=(0xF<<4);

constexpr UINT MBOX_DATA_VER			=(4<<16);
constexpr UINT MBOX_DATA_HALT			=(1<<4);
constexpr UINT MBOX_DATA_ACK			=(1<<1);


//======
// Wifi
//======

constexpr UINT WIFI_TIMEOUT				=100;

constexpr EMMC_FN FN2					={ 2, 512 };

typedef struct
{
WORD Flags;
WORD Type;
UINT Size;
UINT Crc;
}CLM_HEADER;

constexpr WORD CLM_TYPE=2;
constexpr UINT CLM_DATA_MAX=1024;

constexpr WORD CLMF_CLM=(1<<12);
constexpr WORD CLMF_LAST=(1<<2);
constexpr WORD CLMF_FIRST=(1<<1);


//==================
// Con-/Destructors
//==================

WifiAdapter::~WifiAdapter()
{
if(m_ServiceTask)
	m_ServiceTask->Cancel();
GpioHelper::DigitalWrite(GpioArmPin::WifiOn, false);
}


//========
// Common
//========

VOID WifiAdapter::Send(WifiPacket* pkt)
{
SpinLock lock(m_CriticalSection);
m_OutputQueue.Append(pkt);
m_ServicePending.Trigger();
}

Handle<WifiPacket> WifiAdapter::SendAndReceive(WifiPacket* pkt)
{
SpinLock lock(m_CriticalSection);
m_OutputQueue.Append(pkt);
m_ServicePending.Trigger();
m_ResponseReceived.Wait(lock, WIFI_TIMEOUT);
auto response=m_Response;
m_Response=nullptr;
return response;
}


//==========================
// Con-/Destructors Private
//==========================

WifiAdapter::WifiAdapter():
m_Device((EMMC_REGS*)EMMC_BASE),
m_IoWindow(0)
{
Initialize();
}


//================
// Common Private
//================

INT WifiAdapter::GetInt(WifiCmd id)
{
UINT cmd_size=sizeof(WIFI_CMD)+sizeof(INT);
auto pkt=WifiPacket::Create(cmd_size);
auto cmd=pkt->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=id;
cmd->Flags=WifiCmdFlags::Read;
cmd->Length=sizeof(INT);
auto response=SendAndReceive(pkt);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
INT value=0;
pkt->Read(&value, sizeof(INT));
return value;
}

INT WifiAdapter::GetInt(LPCSTR name)
{
INT value=0;
GetVariable(name, &value, sizeof(INT));
return value;
}

UINT WifiAdapter::GetVariable(LPCSTR name, VOID* buf, UINT size)
{
UINT name_size=StringHelper::Length(name)+1;
UINT cmd_size=sizeof(WIFI_CMD)+name_size+size;
auto pkt=WifiPacket::Create(cmd_size);
auto cmd=pkt->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=WifiCmd::GetVariable;
cmd->Flags=WifiCmdFlags::Read;
cmd->Length=name_size+size;
pkt->Write(name, name_size);
auto response=SendAndReceive(pkt);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
pkt->Read(buf, size);
return size;
}

VOID WifiAdapter::Initialize()
{
GpioHelper::SetPinMode(GpioArmPin::WifiOn, GpioArmPinMode::Output);
GpioHelper::DigitalWrite(GpioArmPin::WifiOn, true);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioClk, GpioArmPinMode::Func4);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioCmd, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD0, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD1, GpioArmPinMode::Func3, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD2, GpioArmPinMode::Func4, GpioPullMode::PullUp);
GpioHelper::SetPinMode(GpioArmPin::WifiSdioD3, GpioArmPinMode::Func3, GpioPullMode::PullUp);
m_EmmcHost=EmmcHost::Create(EMMC_BASE, Irq::Wifi);
m_EmmcHost->SetClockRate(EMMC_BASE_CLOCK, EMMC_CLOCK);
m_EmmcHost->Command(EmmcCmd::GoIdle);
UINT op_cond=m_EmmcHost->Command(EmmcCmd::SendOpCond, 0);
for(UINT retry=0; retry<5; retry++)
	{
	if(FlagHelper::Get(op_cond, OP_COND_SUCCESS))
		break;
	Task::Sleep(10);
	op_cond=m_EmmcHost->Command(EmmcCmd::SendOpCond, OP_COND_3V3);
	}
if(!FlagHelper::Get(op_cond, OP_COND_SUCCESS))
	throw DeviceNotReadyException();
UINT rel_addr=m_EmmcHost->Command(EmmcCmd::SendRelAddr);
UINT rca=BitHelper::Get(rel_addr, RELADDR_RCA);
m_EmmcHost->SelectCard(rca);
m_EmmcHost->SetRegister(CCCR_SPEED_CTRL, SPEED_CTRL_EHS);
m_EmmcHost->SetRegister(CCCR_BUS_IFC, BUS_IFC_32BIT);
IoHelper::Set(m_Device->CTRL0, CTRL0_HCTL_HS_EN|CTRL0_HCTL_DWIDTH4);
m_EmmcHost->WriteRegister(CCCR_BLKSIZE_FN1, FN1.BLOCK_SIZE);
m_EmmcHost->WriteRegister(CCCR_IOENABLE, FN1_BIT);
m_EmmcHost->PollRegister(CCCR_IOREADY, FN1_BIT, FN1_BIT);
m_EmmcHost->WriteRegister(SB_PULLUPS, 0);
Reset(ARM_BASE, ARM_IOCTRL_HALT);
wifi_config_size=InitializeConfiguration(wifi_config, wifi_config_size);
IoWrite(FN1, ARM_RAM_BASE, &wifi_firmware, wifi_firmware_size);
IoWrite(FN1, ARM_RAM_BASE+ARM_RAM_SIZE-wifi_config_size, wifi_config, wifi_config_size);
IoWrite(FN1, ARM_RESET_VECTOR, *(UINT*)wifi_firmware);
Reset(ARM_BASE, 0);
m_EmmcHost->PollRegister(SB_CLK_CSR, CLK_CSR_HT_AVAIL, CLK_CSR_HT_AVAIL);
m_EmmcHost->WriteRegister(SB_CLK_CSR, CLK_CSR_FORCE_HT);
IoWrite(FN1, SDIO_MBOX_DATA_OUT, MBOX_DATA_VER);
IoWrite(FN1, SDIO_INT_MASK, INT_HOST|INT_FRAME|INT_FCCHANGE);
IoWrite(FN1, SDIO_INT_STATUS, ~0U);
m_EmmcHost->WriteRegister(CCCR_BLKSIZE_FN2_0, (BYTE)FN2.BLOCK_SIZE);
m_EmmcHost->WriteRegister(CCCR_BLKSIZE_FN2_1, (BYTE)(FN2.BLOCK_SIZE>>8));
m_EmmcHost->SetRegister(CCCR_IOENABLE, FN2_BIT);
m_EmmcHost->PollRegister(CCCR_IOREADY, FN2_BIT, FN2_BIT);
m_EmmcHost->CardIrq.Add(this, &WifiAdapter::OnEmmcHostCardInterrupt);
m_ServiceTask=ServiceTask::Create(this, &WifiAdapter::ServiceTask, "wifi");
Task::Sleep(0);
UploadRegulatory();
GetVariable("cur_etheraddr", &m_MacAddress, MAC_ADDR_SIZE);
SetInt("assoc_listen", 10);
SetInt("bus:txglom", 0);
SetInt("bcn_timeout", 10);
SetInt("assoc_retry_max", 3);
SetVariable("event_msgs", WIFI_EVENT_MASK, sizeof(WIFI_EVENT_MASK));
SetInt(WifiCmd::SetScanChannelTime, 40);
SetInt(WifiCmd::SetScanUnassocTime, 40);
SetInt(WifiCmd::SetScanPassiveTime, 130);
SetInt("roam_off", 1);
SetInt(WifiCmd::SetInfra, 1);
SetInt(WifiCmd::SetPromisc, 0);
SetInt(WifiCmd::Up, 1);
}

UINT WifiAdapter::InitializeConfiguration(BYTE* buf, UINT size)
{
auto src=(LPSTR)buf;
auto dst=src;
UINT dst_pos=0;
BOOL comment=false;
BOOL zero=true;
for(UINT src_pos=0; src_pos<size; src_pos++)
	{
	CHAR c=src[src_pos];
	switch(c)
		{
		case '\0':
		case '\r':
			{
			continue;
			}
		case '\n':
			{
			comment=false;
			if(!zero)
				dst[dst_pos++]=0;
			zero=true;
			continue;
			}
		case '#':
			{
			comment=true;
			continue;
			}
		default:
			{
			if(comment)
				break;
			dst[dst_pos++]=c;
			zero=false;
			continue;
			}
		}
	}
if(dst[dst_pos-1]!=0)
	dst[dst_pos++]=0;
while(dst_pos%4)
	dst[dst_pos++]=0;
UINT token=dst_pos/4;
token=(~token<<16)|(token&0xFFFF);
auto valid=(UINT*)&dst[dst_pos];
*valid=token;
dst_pos+=4;
return dst_pos;
}

VOID WifiAdapter::IoPoll(EMMC_FN fn, UINT addr, UINT mask, UINT value, UINT timeout)
{
IoWindow(addr);
addr&=SB_WND_MASK;
addr|=SB_ADDR_32BIT;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, fn.ID);
BitHelper::Set(arg, IO_RW_ADDR, addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
UINT end=SystemTimer::GetTickCount64()+timeout;
UINT reg=0;
do
	{
	m_EmmcHost->Command(EmmcCmd::IoReadSingle, arg, nullptr, &reg, 1, sizeof(UINT));
	if(BitHelper::Get(reg, mask)==value)
		return;
	Task::Sleep(10);
	}
while(SystemTimer::GetTickCount64()<=end);
throw DeviceNotReadyException();
}

UINT WifiAdapter::IoRead(EMMC_FN fn, UINT addr)
{
IoWindow(addr);
addr&=SB_WND_MASK;
addr|=SB_ADDR_32BIT;
UINT arg=IO_RW_READ;
BitHelper::Set(arg, IO_RW_FUNC, fn.ID);
BitHelper::Set(arg, IO_RW_ADDR, addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
UINT value=0;
m_EmmcHost->Command(EmmcCmd::IoReadSingle, arg, nullptr, &value, 1, sizeof(UINT));
return value;
}

UINT WifiAdapter::IoRead(EMMC_FN fn, UINT addr, VOID* buf, UINT size)
{
auto dst=(BYTE*)buf;
UINT pos=0;
while(pos<size)
	{
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size-pos, SB_WND_SIZE-wnd_pos);
	IoWindow(addr);
	m_EmmcHost->IoRwExtended(fn, IoRwFlags::ReadIncr, wnd_pos|SB_ADDR_32BIT, dst, copy);
	addr+=copy;
	dst+=copy;
	pos+=copy;
	}
return size;
}

VOID WifiAdapter::IoWindow(UINT addr)
{
addr&=~SB_WND_MASK;
if(m_IoWindow==addr)
	return;
m_IoWindow=addr;
m_EmmcHost->WriteRegister(SB_WINDOW_0, addr>>8);
m_EmmcHost->WriteRegister(SB_WINDOW_1, addr>>16);
m_EmmcHost->WriteRegister(SB_WINDOW_2, addr>>24);
}

VOID WifiAdapter::IoWrite(EMMC_FN fn, UINT addr, UINT value)
{
IoWindow(addr);
addr&=SB_WND_MASK;
addr|=SB_ADDR_32BIT;
UINT arg=IO_RW_WRITE;
BitHelper::Set(arg, IO_RW_FUNC, fn.ID);
BitHelper::Set(arg, IO_RW_ADDR, addr);
BitHelper::Set(arg, IO_RW_COUNT, sizeof(UINT));
m_EmmcHost->Command(EmmcCmd::IoWriteSingle, arg, nullptr, &value, 1, sizeof(UINT));
}

VOID WifiAdapter::IoWrite(EMMC_FN fn, UINT addr, VOID const* buf, UINT size)
{
auto src=(BYTE*)buf;
UINT pos=0;
while(pos<size)
	{
	IoWindow(addr);
	UINT wnd_pos=addr&SB_WND_MASK;
	UINT copy=TypeHelper::Min(size-pos, SB_WND_SIZE-wnd_pos);
	m_EmmcHost->IoRwExtended(fn, IoRwFlags::WriteIncr, wnd_pos|SB_ADDR_32BIT, src, copy);
	addr+=copy;
	src+=copy;
	pos+=copy;
	}
}

VOID WifiAdapter::OnEmmcHostCardInterrupt()
{
m_ServicePending.Trigger();
}

Handle<WifiPacket> WifiAdapter::ReadPacket()
{
WIFI_HEADER header;
WifiRead(&header, sizeof(WIFI_HEADER));
UINT len=header.Length;
if(len)
	{
	UINT len_chk=header.LengthChk;
	if(len_chk!=(len^0xFFFF)||len<sizeof(WIFI_HEADER))
		throw DeviceNotReadyException();
	}
if(!len)
	return nullptr;
auto type=(WifiPacketType)BitHelper::Get(header.Flags, HEADER_FLAGS_TYPE);
switch(type)
	{
	case WifiPacketType::Response:
		{
		m_Response=WifiPacket::Create(header);
		if(len>sizeof(WIFI_HEADER))
			{
			UINT copy=TypeHelper::AlignUp(len-sizeof(WIFI_HEADER), 4U);
			auto data=m_Response->Write<BYTE>(copy);
			WifiRead(data, copy);
			}
		m_ResponseReceived.Trigger();
		break;
		}
	}
return nullptr;
}

VOID WifiAdapter::Reset(UINT addr, UINT flags)
{
IoWrite(FN1, addr+RESET, RESET_BIT);
Task::Sleep(10);
IoWrite(FN1, addr+IOCTRL, flags|IOCTRL_FGC|IOCTRL_CLOCK);
IoWrite(FN1, addr+RESET, 0);
Task::Sleep(10);
IoWrite(FN1, addr+IOCTRL, flags|IOCTRL_CLOCK);
}

VOID WifiAdapter::ServiceTask()
{
IoHelper::Set(m_Device->IRQ_MASK, IRQF_CARD);
IoHelper::Set(m_Device->IRQ_EN, IRQF_CARD);
m_EmmcHost->WriteRegister(CCCR_INT_EN, FN2_BIT|FN1_BIT|FN0_BIT);
auto task=Task::Get();
Handle<WifiPacket> output;
SpinLock lock(m_CriticalSection);
while(!task->Cancelled)
	{
	m_ServicePending.Wait(lock);
	output=m_OutputQueue.Dequeue();
	lock.Unlock();
	UINT int_pending=m_EmmcHost->ReadRegister(CCCR_INT_PENDING);
	UINT host_flags=0;
	if(int_pending)
		{
		UINT int_status=IoRead(FN1, SDIO_INT_STATUS);
		host_flags=BitHelper::Get(int_status, INT_HOST);
		IoHelper::Write(m_Device->IRQ, IRQF_CARD);
		}
	if(host_flags)
		{
		IoWrite(FN1, SDIO_INT_STATUS, host_flags);
		if(BitHelper::Get(host_flags, INT_FRAME))
			{
			auto input=ReadPacket();
			if(input)
				PacketReceived(this, input);
			}
		}
	if(output)
		{
		UINT size=output->GetSize();
		auto buf=output->Begin();
		WifiWrite(buf, size);
		output=nullptr;
		}
	lock.Lock();
	}
}

VOID WifiAdapter::SetInt(WifiCmd id, INT value)
{
UINT cmd_size=sizeof(WIFI_CMD)+sizeof(INT);
auto pkt=WifiPacket::Create(cmd_size);
auto cmd=pkt->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=id;
cmd->Flags=WifiCmdFlags::Write;
cmd->Length=sizeof(INT);
pkt->Write(&value, sizeof(INT));
auto response=SendAndReceive(pkt);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

VOID WifiAdapter::SetInt(LPCSTR name, INT value)
{
SetVariable(name, &value, sizeof(INT));
}

VOID WifiAdapter::SetVariable(LPCSTR name, VOID const* buf, UINT size)
{
UINT name_size=StringHelper::Length(name)+1;
UINT cmd_size=sizeof(WIFI_CMD)+name_size+size;
auto pkt=WifiPacket::Create(cmd_size);
auto cmd=pkt->Write<WIFI_CMD>();
MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
cmd->Command=WifiCmd::SetVariable;
cmd->Flags=WifiCmdFlags::Write;
cmd->Length=name_size+size;
pkt->Write(name, name_size);
pkt->Write(buf, size);
auto response=SendAndReceive(pkt);
cmd=response->Read<WIFI_CMD>();
if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
	throw InvalidArgumentException();
}

VOID WifiAdapter::UploadRegulatory()
{
CHAR name[]="clmload";
UINT name_size=sizeof(name);
WORD clm_flags=CLMF_CLM|CLMF_FIRST;
WORD request=0;
WORD pos=0;
while(pos<wifi_clm_size)
	{
	WORD copy=TypeHelper::Min((WORD)(wifi_clm_size-pos), CLM_DATA_MAX);
	if(pos+copy==wifi_clm_size)
		BitHelper::Set(clm_flags, CLMF_LAST);
	WORD clm_size=sizeof(CLM_HEADER)+copy;
	WORD cmd_size=sizeof(WIFI_CMD)+name_size+clm_size;
	auto pkt=WifiPacket::Create(cmd_size);
	auto cmd=pkt->Write<WIFI_CMD>();
	MemoryHelper::Zero(cmd, sizeof(WIFI_CMD));
	cmd->Command=WifiCmd::SetVariable;
	cmd->Flags=WifiCmdFlags::Write;
	cmd->Id=++request;
	cmd->Length=name_size+clm_size;
	pkt->Write(name, name_size);
	auto clm=pkt->Write<CLM_HEADER>();
	MemoryHelper::Zero(clm, sizeof(CLM_HEADER));
	clm->Flags=clm_flags;
	clm->Size=copy;
	clm->Type=CLM_TYPE;
	pkt->Write(&wifi_clm[pos], copy);
	auto response=SendAndReceive(pkt);
	cmd=response->Read<WIFI_CMD>();
	if(FlagHelper::Get(cmd->Flags, WifiCmdFlags::Error))
		throw InvalidArgumentException();
	BitHelper::Clear(clm_flags, CLMF_FIRST);
	pos+=copy;
	}
}

VOID WifiAdapter::WifiRead(VOID* buf, UINT size)
{
assert(size%4==0);
auto dst=(BYTE*)buf;
UINT pos=0;
while(pos<size)
	{
	UINT copy=TypeHelper::Min(size-pos, WIFI_PACKET_MAX);
	m_EmmcHost->IoRwExtended(FN2, IoRwFlags::Read, 0, dst, copy);
	dst+=copy;
	pos+=copy;
	}
}

VOID WifiAdapter::WifiWrite(VOID const* buf, UINT size)
{
assert(size%4==0);
auto src=(BYTE*)buf;
UINT pos=0;
while(pos<size)
	{
	UINT copy=TypeHelper::Min(size-pos, WIFI_PACKET_MAX);
	m_EmmcHost->IoRwExtended(FN2, IoRwFlags::Write, 0, src, copy);
	src+=copy;
	pos+=copy;
	}
}

}}