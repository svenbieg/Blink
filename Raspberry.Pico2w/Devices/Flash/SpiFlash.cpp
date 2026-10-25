//==============
// SpiFlash.cpp
//==============

#include "SpiFlash.h"


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/Sio.h"
#include "Devices/Timers/SystemTimer.h"
#include "Devices/Peripherals.h"
#include "Devices/IoHelper.h"
#include "Exception.h"

using namespace Concurrency;
using namespace Devices::Dma;
using namespace Devices::System;
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


//===============
// ROM-Functions
//===============

consteval UINT ROM_FN(CHAR c1, CHAR c2)
{
return (UINT)c1|((UINT)c2<<8);
}

static const UINT FLASH_CONNECT_FN	=ROM_FN('I', 'F');
static const UINT FLASH_ERASE_FN	=ROM_FN('R', 'E');
static const UINT FLASH_EXIT_FN		=ROM_FN('E', 'X');
static const UINT FLASH_FLUSH_FN	=ROM_FN('F', 'C');
static const UINT FLASH_PROGRAM_FN	=ROM_FN('R', 'P');


//=====
// QMI
//=====

typedef struct
{
RW32 TIMING;
RW32 RFMT;
RW32 RCMD;
RW32 WFMT;
RW32 WCMD;
}QMI_WND;

typedef struct
{
RW32 DIRECT_CSR;
WO32 DIRECT_TX;
RO32 DIRECT_RX;
QMI_WND WND[2];
RW32 ATRANS[8];
}QMI_REGS;


//======
// Pads
//======

static const UINT PADS_QSPI_COUNT=6;

typedef struct
{
RW32 VSEL;
RW32 IO[PADS_QSPI_COUNT];
}PADS_QSPI_REGS;


//========
// Volume
//========

VOID SpiFlash::Erase(UINT block_id)
{
assert(!Interrupts::Active());
if(block_id>=TOTAL_SIZE/BLOCK_SIZE)
	throw OutOfRangeException();
WriteLock lock(m_Mutex);
Interrupts::Disable();
Cpu::SuspendAll();
DoErase(block_id*BLOCK_SIZE, BLOCK_SIZE);
Cpu::ResumeAll();
Interrupts::Enable();
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
assert(block_id<BLOCK_COUNT);
assert(page_id<BLOCK_SIZE/PAGE_SIZE);
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
m_DmaChannel->Read(DmaRequest::XipStream, (RO32*)XIP_AUX_BASE, buf, PAGE_SIZE);
return page;
}

VOID SpiFlash::Write(UINT block_id, WORD page_id, WORD pos, VOID const* buf, WORD size)
{
assert(!Interrupts::Active());
assert(block_id<BLOCK_COUNT);
assert(page_id<BLOCK_SIZE/PAGE_SIZE);
assert(pos+size<=PAGE_SIZE);
assert(buf!=nullptr);
assert(size>0);
UINT offset=block_id*BLOCK_SIZE+page_id*PAGE_SIZE;
WriteLock lock(m_Mutex);
if(pos>0||size<PAGE_SIZE)
	{
	BYTE* page_buf=m_Buffer->Page;
	MemoryHelper::FillT((UINT*)page_buf, PAGE_SIZE/sizeof(UINT), 0xFFFFFFFF);
	MemoryHelper::Copy(&page_buf[pos], buf, size);
	Interrupts::Disable();
	Cpu::SuspendAll();
	DoWrite(offset, page_buf, PAGE_SIZE);
	Cpu::ResumeAll();
	Interrupts::Enable();
	}
else
	{
	assert((SIZE_T)buf%sizeof(UINT)==0);
	Interrupts::Disable();
	Cpu::SuspendAll();
	DoWrite(offset, buf, PAGE_SIZE);
	Cpu::ResumeAll();
	Interrupts::Enable();
	}
}


//==========================
// Con-/Destructors Private
//==========================

SpiFlash::SpiFlash(BYTE* buf, SIZE_T size):
m_Boot2((boot2_fn)((SIZE_T)buf|1)),
m_Buffer((FLASH_BUFFER*)buf)
{
MemoryHelper::CopyT((COPY_T*)m_Buffer->Boot2, (COPY_T const*)BOOT_RAM_BASE, BOOT2_SIZE/sizeof(COPY_T));
m_DmaChannel=DmaChannel::Create();
m_FlashConnect=(flash_connect_fn)LookupRomFunction(FLASH_CONNECT_FN);
m_FlashErase=(flash_erase_fn)LookupRomFunction(FLASH_ERASE_FN);
m_FlashExit=(flash_exit_fn)LookupRomFunction(FLASH_EXIT_FN);
m_FlashFlush=(flash_flush_fn)LookupRomFunction(FLASH_FLUSH_FN);
m_FlashProgram=(flash_program_fn)LookupRomFunction(FLASH_PROGRAM_FN);
}


//================
// Common Private
//================

__ISR VOID SpiFlash::DoErase(UINT offset, SIZE_T size)noexcept
{
static const BYTE CMD_ERASE_SECTOR=0x20;
auto qmi=(QMI_REGS*)XIP_QMI_BASE;
UINT qmi1_timing=qmi->WND[1].TIMING;
UINT qmi1_rfmt=qmi->WND[1].RFMT;
UINT qmi1_rcmd=qmi->WND[1].RCMD;
auto pads=(PADS_QSPI_REGS*)PADS_QSPI_BASE;
UINT pad_state[PADS_QSPI_COUNT];
for(UINT u=0; u<PADS_QSPI_COUNT; u++)
	pad_state[u]=pads->IO[u];
m_FlashConnect();
m_FlashExit();
m_FlashErase(offset, size, BLOCK_SIZE, CMD_ERASE_SECTOR);
m_FlashFlush();
m_Boot2();
qmi->WND[1].TIMING=qmi1_timing;
qmi->WND[1].RFMT=qmi1_rfmt;
qmi->WND[1].RCMD=qmi1_rcmd;
for(UINT u=0; u<PADS_QSPI_COUNT; u++)
	pads->IO[u]=pad_state[u];
}

__ISR VOID SpiFlash::DoWrite(UINT offset, VOID const* buf, SIZE_T size)noexcept
{
auto qmi=(QMI_REGS*)XIP_QMI_BASE;
UINT qmi1_timing=qmi->WND[1].TIMING;
UINT qmi1_rfmt=qmi->WND[1].RFMT;
UINT qmi1_rcmd=qmi->WND[1].RCMD;
auto pads=(PADS_QSPI_REGS*)PADS_QSPI_BASE;
UINT pad_state[PADS_QSPI_COUNT];
for(UINT u=0; u<PADS_QSPI_COUNT; u++)
	pad_state[u]=pads->IO[u];
m_FlashConnect();
m_FlashExit();
m_FlashProgram(offset, (BYTE const*)buf, size);
m_FlashFlush();
m_Boot2();
qmi->WND[1].TIMING=qmi1_timing;
qmi->WND[1].RFMT=qmi1_rfmt;
qmi->WND[1].RCMD=qmi1_rcmd;
for(UINT u=0; u<PADS_QSPI_COUNT; u++)
	pads->IO[u]=pad_state[u];
}

VOID* SpiFlash::LookupRomFunction(UINT code)
{
using rom_table_lookup_fn=VOID* (*)(UINT Code, UINT Mask);
static const SIZE_T LOOKUP_FN_ADDR=0x16;
auto lookup=(rom_table_lookup_fn)(SIZE_T)(*(volatile WORD*)LOOKUP_FN_ADDR);
assert(lookup);
static const UINT FLAG_FUNC_ARM_SEC=0x04;
VOID* function=lookup(code, FLAG_FUNC_ARM_SEC);
assert(function);
return function;
}

}}