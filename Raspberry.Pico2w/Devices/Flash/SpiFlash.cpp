//==============
// SpiFlash.cpp
//==============

#include "SpiFlash.h"


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/Peripherals.h"
#include "Devices/IoHelper.h"

using namespace Concurrency;
using namespace Devices::Dma;
using namespace Devices::Timers;
using namespace Storage;


//===========
// Namespace
//===========

namespace Devices {
	namespace Flash {


//=============
// XIP-Control
//=============

typedef struct
{
RW32 CTRL;
UINT RES0;
RO32 STAT;
RW32 CTR_HIT;
RW32 CTR_ACC;
RW32 STREAM_ADDR;
RW32 STREAM_CTR;
RW32 STREAM_FIFO;
}XIP_CTRL_REGS;

const UINT STAT_FIFO_FULL=(1<<2);
const UINT STAT_FIFO_EMPTY=(1<<1);


//========
// Volume
//========

VOID SpiFlash::Erase(UINT block_id)
{
throw NotImplementedException();
}

UINT SpiFlash::GetBlockSize()
{
return BLOCK_SIZE;
}

WORD SpiFlash::GetPageSize(WORD* spare_ptr)
{
if(spare_ptr)
	*spare_ptr=0;
return PAGE_SIZE;
}

UINT64 SpiFlash::GetSize()
{
return TOTAL_SIZE;
}

Handle<Page> SpiFlash::ReadPage(UINT block_id, WORD page_id)
{
WriteLock lock(m_Mutex);
auto xip=(XIP_CTRL_REGS*)XIP_CTRL_BASE;
for(UINT retry=0; !IoHelper::Read(xip->STAT, STAT_FIFO_EMPTY); retry++)
	{
	if(retry>64)
		throw DeviceNotReadyException();
	IoHelper::Read(xip->STREAM_FIFO);
	}
auto page=Page::Create(this);
auto buf=page->Begin();
UINT read_addr=block_id*BLOCK_SIZE+page_id*PAGE_SIZE;
IoHelper::Write(xip->STREAM_ADDR, read_addr);
IoHelper::Write(xip->STREAM_CTR, PAGE_SIZE/sizeof(UINT));
m_DmaChannel->BeginRead(DmaRequest::XipStream, (RO32*)XIP_AUX_BASE, buf, PAGE_SIZE);
m_DmaChannel->Wait();
return page;
}

VOID SpiFlash::Write(UINT block_id, WORD page_id, WORD pos, VOID const* buf, WORD size)
{
throw NotImplementedException();
}


//==========================
// Con-/Destructors Private
//==========================

SpiFlash::SpiFlash()
{
m_DmaChannel=DmaChannel::Create();
}

}}