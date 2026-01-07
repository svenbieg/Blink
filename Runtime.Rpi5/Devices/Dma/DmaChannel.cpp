//================
// DmaChannel.cpp
//================

#include "DmaChannel.h"


//=======
// Using
//=======

#include "Devices/System/Interrupts.h"
#include "Devices/IoHelper.h"
#include "BitHelper.h"
#include <base.h>

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Dma {


//==========
// Settings
//==========

constexpr UINT DMA_CHANNEL_COUNT=12;
constexpr UINT DMA_CHANNEL_MAX=11;
constexpr UINT DMA_CHANNEL_MIN=6;
constexpr UINT DMA_COPY_MAX=0x3FFFFFFF;


//===============
// Control-Block
//===============

typedef struct
{
UINT TI;
UINT SRC;
UINT SRCI;
UINT DST;
UINT DSTI;
UINT LEN;
UINT NEXT;
UINT DEBUG;
}DMA_CB;

constexpr UINT DSTI_INC=(1<<12);

constexpr UINT SRCI_INC=(1<<12);

constexpr BITS TI_DST_WAIT={ 0xFF, 24 };
constexpr BITS TI_SRC_WAIT={ 0xFF, 16 };
constexpr UINT TI_DST_DREQ=(1<<15);
constexpr UINT TI_SRC_DREQ=(1<<14);
constexpr BITS TI_PERMAP={ 0x1F, 9 };
constexpr UINT TI_WAIT_RD_RESP=(1<<3);
constexpr UINT TI_WAIT_RESP=(1<<2);
constexpr UINT TI_2DMODE=(1<<1);
constexpr UINT TI_INTEN=(1<<0);


//=========
// Channel
//=========

typedef struct
{
RW32 CS;
RW32 CB_ADDR;
RW32 RES0;
RW32 DEBUG;
DMA_CB CB;
RW32 RES1[52];
}DMA_CHANNEL_REGS;

constexpr UINT CB_ADDR_SHIFT=5;

constexpr UINT CS_HALT=(1<<31);
constexpr UINT CS_ABORT=(1<<30);
constexpr UINT CS_DISDEBUG(1<<29);
constexpr UINT CS_WAIT_FOR_WRITES=(1<<28);
constexpr UINT CS_TRANSACTIONS=(1<<25);
constexpr BITS CS_PANIC_PRIORITY={ 0xF, 20 };
constexpr UINT CS_PANIC_PRIORITY_DEFAULT=15;
constexpr BITS CS_PRIORITY={ 0xF, 16 };
constexpr UINT CS_PRIORITY_DEFAULT=1;
constexpr UINT CS_ERROR=(1<<10);
constexpr UINT CS_PROT=(3<<8);
constexpr UINT CS_WAITING_FOR_WRITES=(1<<7);
constexpr UINT CS_DREQ_PAUSED=(1<<6);
constexpr UINT CS_WR_PAUSED=(1<<5);
constexpr UINT CS_RD_PAUSED=(1<<4);
constexpr UINT CS_DREQ=(1<<3);
constexpr UINT CS_INT=(1<<2);
constexpr UINT CS_END=(1<<1);
constexpr UINT CS_ACTIVE=(1<<0);

constexpr UINT DEBUG_RESET=(1<<23);
constexpr UINT DEBUG_READ_CB_ERR=(1<<3);
constexpr UINT DEBUG_READ_ERR=(1<<2);
constexpr UINT DEBUG_FIFO_ERR=(1<<1);
constexpr UINT DEBUG_WRITE_ERR=(1<<0);


//======
// Host
//======

typedef struct
{
DMA_CHANNEL_REGS CH[DMA_CHANNEL_COUNT];
}DMA_REGS;


//==================
// Con-/Destructors
//==================

Handle<DmaChannel> DmaChannel::Create(DmaDevice device)
{
UINT id=GetChannel();
Handle<DmaChannel> dma_channel;
try
	{
	dma_channel=Object::CreateEx<DmaChannel>(sizeof(DMA_CB), sizeof(DMA_CB), device, id);
	}
catch(Exception e)
	{
	WriteLock lock(s_Mutex);
	FlagHelper::Clear(s_Used, 1<<id);
	throw e;
	}
return dma_channel;
}

DmaChannel::~DmaChannel()
{
auto dma=(DMA_REGS*)AXI_DMA_BASE;
auto ch=&dma->CH[m_Id];
IoHelper::Write(ch->CS, 0);
Irq irq=(Irq)((UINT)Irq::Dma0+m_Id);
Interrupts::SetHandler(irq, nullptr);
WriteLock lock(s_Mutex);
FlagHelper::Clear(s_Used, 1<<m_Id);
}


//========
// Common
//========

VOID DmaChannel::Read(volatile UINT* reg, VOID* buf, SIZE_T size, UINT timeout)
{
assert(buf);
assert(size>0);
assert(size<=DMA_COPY_MAX);
auto cb=(DMA_CB*)m_ControlBlock;
MemoryHelper::ZeroT(cb);
BitHelper::Set(cb->TI, TI_INTEN|TI_WAIT_RD_RESP|TI_WAIT_RESP);
if(m_Device!=DmaDevice::None)
	{
	BitHelper::Set(cb->TI, TI_SRC_DREQ);
	BitHelper::Set(cb->TI, TI_PERMAP, (UINT)m_Device);
	}
SIZE_T source=(SIZE_T)reg;
BitHelper::Set(cb->SRC, (UINT)source);
BitHelper::Set(cb->SRCI, (UINT)(source>>32));
SIZE_T dest=(SIZE_T)buf;
BitHelper::Set(cb->DST, (UINT)dest);
BitHelper::Set(cb->DSTI, (UINT)(dest>>32));
BitHelper::Set(cb->DSTI, DSTI_INC);
BitHelper::Set(cb->LEN, (UINT)size);
Cpu::InvalidateDataCache((SIZE_T)cb, sizeof(DMA_CB));
Cpu::InvalidateDataCache((SIZE_T)buf, size);
Cpu::DataSyncBarrier();
SpinLock lock(m_CriticalSection);
auto dma=(DMA_REGS*)AXI_DMA_BASE;
auto ch=&dma->CH[m_Id];
UINT cb_addr=(SIZE_T)m_ControlBlock>>CB_ADDR_SHIFT;
IoHelper::Write(ch->CB_ADDR, cb_addr);
UINT cs=CS_ACTIVE|CS_PROT;
BitHelper::Set(cs, CS_PRIORITY, CS_PRIORITY_DEFAULT);
BitHelper::Set(cs, CS_PANIC_PRIORITY, CS_PANIC_PRIORITY_DEFAULT);
IoHelper::Write(ch->CS, cs);
m_Signal.Wait(lock, timeout);
IoHelper::Write(ch->CS, 0);
}

VOID DmaChannel::Write(volatile UINT* reg, VOID const* buf, SIZE_T size, UINT timeout)
{
assert(buf);
assert(size>0);
assert(size<=DMA_COPY_MAX);
auto cb=(DMA_CB*)m_ControlBlock;
MemoryHelper::ZeroT(cb);
BitHelper::Set(cb->TI, TI_INTEN|TI_WAIT_RESP);
if(m_Device!=DmaDevice::None)
	{
	BitHelper::Set(cb->TI, TI_DST_DREQ);
	BitHelper::Set(cb->TI, TI_PERMAP, (UINT)m_Device);
	}
SIZE_T source=(SIZE_T)buf;
BitHelper::Set(cb->SRC, (UINT)source);
BitHelper::Set(cb->SRCI, (UINT)(source>>32));
BitHelper::Set(cb->SRCI, SRCI_INC);
SIZE_T dest=(SIZE_T)reg;
BitHelper::Set(cb->DST, (UINT)dest);
BitHelper::Set(cb->DSTI, (UINT)(dest>>32));
BitHelper::Set(cb->LEN, (UINT)size);
Cpu::InvalidateDataCache((SIZE_T)cb, sizeof(DMA_CB));
Cpu::CleanDataCache(source, size);
Cpu::DataSyncBarrier();
SpinLock lock(m_CriticalSection);
auto dma=(DMA_REGS*)AXI_DMA_BASE;
auto ch=&dma->CH[m_Id];
UINT cb_addr=(SIZE_T)m_ControlBlock>>CB_ADDR_SHIFT;
IoHelper::Write(ch->CB_ADDR, cb_addr);
UINT cs=CS_ACTIVE|CS_PROT;
BitHelper::Set(cs, CS_PRIORITY, CS_PRIORITY_DEFAULT);
BitHelper::Set(cs, CS_PANIC_PRIORITY, CS_PANIC_PRIORITY_DEFAULT);
IoHelper::Write(ch->CS, cs);
m_Signal.Wait(lock, timeout);
IoHelper::Write(ch->CS, 0);
}


//==========================
// Con-/Destructors Private
//==========================

DmaChannel::DmaChannel(VOID* buf, SIZE_T size, DmaDevice device, UINT id):
m_ControlBlock(buf),
m_Device(device),
m_Id(id)
{
Irq irq=(Irq)((UINT)Irq::Dma0+m_Id);
Interrupts::SetHandler(irq, &DmaChannel::HandleInterrupt, this);
}

Mutex DmaChannel::s_Mutex;
UINT DmaChannel::s_Used=0;


//================
// Common Private
//================

UINT DmaChannel::GetChannel()
{
WriteLock lock(s_Mutex);
for(UINT id=DMA_CHANNEL_MIN; id<=DMA_CHANNEL_MAX; id++)
	{
	if(!FlagHelper::Get(s_Used, 1<<id))
		{
		FlagHelper::Set(s_Used, 1<<id);
		return id;
		}
	}
throw DeviceNotReadyException();
}

VOID DmaChannel::HandleInterrupt(VOID* param)
{
auto dma_channel=(DmaChannel*)param;
dma_channel->OnInterrupt();
}

VOID DmaChannel::OnInterrupt()
{
SpinLock lock(m_CriticalSection);
auto dma=(DMA_REGS*)AXI_DMA_BASE;
auto ch=&dma->CH[m_Id];
UINT cs=IoHelper::Read(ch->CS);
IoHelper::Write(ch->CS, cs);
Status status=Status::Success;
if(FlagHelper::Get(cs, CS_ERROR))
	status=Status::DeviceNotReady;
m_Signal.Trigger(status);
}

}}