//================
// DmaChannel.cpp
//================

#include "DmaChannel.h"


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/IoHelper.h"
#include "BitHelper.h"
#include <base.h>

using namespace Concurrency;
using namespace Devices::System;
using namespace Devices::Timers;


//===========
// Namespace
//===========

namespace Devices {
	namespace Dma {


//==========
// Settings
//==========

const UINT DMA_CHANNEL_COUNT=4;


//===========
// Registers
//===========

typedef struct
{
RW32 READ_ADDR;
RW32 WRITE_ADDR;
RW32 TRANSF_COUNT;
RW32 CTRL_TRIG;
RW32 AL1_CTRL;
RW32 AL1_READ_ADDR;
RW32 AL1_WRITE_ADDR;
RW32 AL1_TRANSF_COUNT_TRIG;
RW32 AL2_CTRL;
RW32 AL2_TRANSF_COUNT;
RW32 AL2_READ_ADDR;
RW32 AL2_WRITE_ADDR_TRIG;
RW32 AL3_CTRL;
RW32 AL3_WRITE_ADDR;
RW32 AL3_TRANSF_COUNT;
RW32 AL3_READ_ADDR_TRIG;
}DMA_CHANNEL_REGS;

const UINT CTRL_ERR_AHB			=(1<<31);
const UINT CTRL_ERR_READ		=(1<<30);
const UINT CTRL_ERR_WRITE		=(1<<29);
const UINT CTRL_ERR				=(7<<29);
const UINT CTRL_BUSY			=(1<<26);
const BITS CTRL_BSWAP			={ 0x01, 24 };
const BITS CTRL_DREQ			={ 0x3F, 17 };
const BITS CTRL_CHAIN_TO		={ 0x0F, 13 };
const UINT CTRL_INCR_WRITE		=(1<<6);
const UINT CTRL_INCR_READ		=(1<<4);
const BITS CTRL_DATA_SIZE		={ 0x03, 2 };
const UINT CTRL_EN				=(1<<0);

enum DATA_SIZE
{
DATA_SIZE_8BIT,
DATA_SIZE_16BIT,
DATA_SIZE_32BIT
};

typedef struct
{
UINT RES;
RW32 INTE;
RW32 INTF;
RW32 INTS;
}DMA_IRQ_REGS;

typedef struct
{
RW32 BASE;
RW32 LIMIT;
}DMA_MPU_REGS;

typedef struct
{
DMA_CHANNEL_REGS CH[16];
union
	{
	RW32 INTR;
	DMA_IRQ_REGS IRQ_CTRL[DMA_CHANNEL_COUNT];
	};
RW32 TIMER[DMA_CHANNEL_COUNT];
WO32 MULTI_CH_TRIG;
RW32 SNIFF_CTRL;
RW32 SNIFF_DATA;
UINT RES0;
RO32 FIFO_LEVELS;
WO32 ABORT;
RO32 CHANNEL_COUNT;
UINT RES1[5];
RW32 SECCFG[16];
RW32 SECCFG_IRQ[DMA_CHANNEL_COUNT];
RW32 SECCFG_MISC;
UINT RES2[11];
RW32 MPU_CTRL;
DMA_MPU_REGS MPU_REGION[8];
}DMA_REGS;


//==================
// Con-/Destructors
//==================

Handle<DmaChannel> DmaChannel::Create()
{
using System=Devices::System::System;
WriteLock lock(s_Mutex);
if(s_Used==0)
	System::Enable(ResetDevice::Dma);
UINT id=GetChannel();
Handle<DmaChannel> dma_channel;
try
	{
	dma_channel=Object::Create<DmaChannel>(id);
	}
catch(Exception e)
	{
	BitHelper::Clear(s_Used, 1U<<id);
	throw e;
	}
return dma_channel;
}

DmaChannel::~DmaChannel()
{
Irq irq=(Irq)((UINT)Irq::Dma0+m_Id);
Interrupts::SetHandler(irq, nullptr);
WriteLock lock(s_Mutex);
BitHelper::Clear(s_Used, 1<<m_Id);
}


//========
// Common
//========

VOID DmaChannel::Abort()
{
auto dma=(DMA_REGS*)DMA_BASE;
IoHelper::Write(dma->ABORT, 1U<<m_Id);
IoHelper::Retry(dma->CH[m_Id].CTRL_TRIG, CTRL_BUSY, 0);
}

VOID DmaChannel::BeginRead(DmaRequest dreq, RO32* reg, UINT* buf, SIZE_T count)
{
assert(buf);
assert(count>0);
UINT ctrl=m_Control;
BitHelper::Set(ctrl, CTRL_INCR_READ|CTRL_INCR_WRITE, CTRL_INCR_WRITE);
BitHelper::Set(ctrl, CTRL_DREQ, (UINT)dreq);
auto dma=(DMA_REGS*)DMA_BASE;
dma->CH[m_Id].READ_ADDR=(SIZE_T)reg;
dma->CH[m_Id].WRITE_ADDR=(SIZE_T)buf;
dma->CH[m_Id].TRANSF_COUNT=count;
SpinLock lock(m_CriticalSection);
m_Status=Status::Pending;
dma->CH[m_Id].CTRL_TRIG=ctrl;
}

VOID DmaChannel::SetByteSwap(BOOL swap)
{
BitHelper::Set(m_Control, CTRL_BSWAP, swap);
}

VOID DmaChannel::BeginWrite(DmaRequest dreq, RW32* reg, UINT const* buf, SIZE_T count)
{
assert(buf);
assert(count>0);
UINT ctrl=m_Control;
BitHelper::Set(ctrl, CTRL_INCR_READ|CTRL_INCR_WRITE, CTRL_INCR_READ);
BitHelper::Set(ctrl, CTRL_DREQ, (UINT)dreq);
auto dma=(DMA_REGS*)DMA_BASE;
dma->CH[m_Id].READ_ADDR=(SIZE_T)buf;
dma->CH[m_Id].WRITE_ADDR=(SIZE_T)reg;
dma->CH[m_Id].TRANSF_COUNT=count;
SpinLock lock(m_CriticalSection);
m_Status=Status::Pending;
dma->CH[m_Id].CTRL_TRIG=ctrl;
}

VOID DmaChannel::Wait(UINT timeout)
{
SpinLock lock(m_CriticalSection);
if(m_Status==Status::Pending)
	m_Signal.Wait(lock, timeout);
if(m_Status!=Status::Success)
	throw DeviceNotReadyException();
}


//==========================
// Con-/Destructors Private
//==========================

DmaChannel::DmaChannel(UINT id):
m_Control(0),
m_Id(id),
m_Status(Status::Success)
{
BitHelper::Set(m_Control, CTRL_CHAIN_TO, m_Id);
BitHelper::Set(m_Control, CTRL_DATA_SIZE, DATA_SIZE_32BIT);
BitHelper::Set(m_Control, CTRL_EN);
Irq irq=(Irq)((UINT)Irq::Dma0+m_Id);
Interrupts::SetHandler(irq, this, &DmaChannel::OnInterrupt);
auto dma=(DMA_REGS*)DMA_BASE;
IoHelper::Set(dma->IRQ_CTRL[m_Id].INTE, 1U<<m_Id);
}

Mutex DmaChannel::s_Mutex;
UINT DmaChannel::s_Used=0;


//================
// Common Private
//================

UINT DmaChannel::GetChannel()
{
for(UINT id=0; id<DMA_CHANNEL_COUNT; id++)
	{
	UINT mask=1U<<id;
	if(!BitHelper::Get(s_Used, mask))
		{
		BitHelper::Set(s_Used, mask);
		return id;
		}
	}
throw DeviceNotReadyException();
}

VOID DmaChannel::OnInterrupt()
{
auto dma=(DMA_REGS*)DMA_BASE;
IoHelper::Set(dma->IRQ_CTRL[m_Id].INTS, 1U<<m_Id);
Status status=Status::Success;
if(IoHelper::Read(dma->CH[m_Id].CTRL_TRIG, CTRL_ERR))
	status=Status::DeviceNotReady;
SpinLock lock(m_CriticalSection);
m_Status=status;
m_Signal.Trigger(status);
}

}}