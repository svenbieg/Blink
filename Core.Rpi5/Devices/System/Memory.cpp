//============
// Memory.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include "Devices/Arm/MailBox.h"
#include "Devices/System/Cpu.h"
#include "BitHelper.h"
#include "Memory.h"
#include "Settings.h"

using namespace Devices::Arm;
using namespace Devices::System;

extern BYTE __bss_start;
extern BYTE __bss_end;

extern BYTE __heap_start;

extern heap_handle_t g_heap;

typedef VOID (*CTOR_PTR)();

extern CTOR_PTR __init_array_start;
extern CTOR_PTR __init_array_end;

extern BYTE __page_table_start;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//============
// Page-Table
//============

constexpr SIZE_T PAGE_TABLE_ENTRIES=512;
constexpr SIZE_T PAGE_TABLE_SIZE=4096;
constexpr SIZE_T REGION_SIZE=0x40000000; // 0x40000000 (1GB)

constexpr UINT PAGE_ATT_CACHED=0;
constexpr UINT PAGE_ATT_UNCACHED=1;
constexpr UINT PAGE_ATT_DEVICE=2;

constexpr SIZE_T ENTRY_PXN=(1ULL<<53);
constexpr SIZE_T ENTRY_UXN=(1ULL<<54);
constexpr SIZE_T ENTRY_ACC=(1<<10);
constexpr SIZE_T ENTRY_SHARE_OUT=(2<<8);
constexpr SIZE_T ENTRY_SHARE_IN=(3<<8);
constexpr SIZE_T ENTRY_ATT_CACHED=(PAGE_ATT_CACHED<<2);
constexpr SIZE_T ENTRY_ATT_UNCACHED=(PAGE_ATT_UNCACHED<<2);
constexpr SIZE_T ENTRY_ATT_DEVICE=(PAGE_ATT_DEVICE<<2);
constexpr SIZE_T ENTRY_TYPE_BLOCK=1;

constexpr SIZE_T ENTRY_CACHED=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_CACHED|ENTRY_TYPE_BLOCK);
constexpr SIZE_T ENTRY_UNCACHED=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_UNCACHED|ENTRY_TYPE_BLOCK);
constexpr SIZE_T ENTRY_DEVICE=(ENTRY_PXN|ENTRY_UXN|ENTRY_ACC|ENTRY_SHARE_OUT|ENTRY_ATT_DEVICE|ENTRY_TYPE_BLOCK);

static SIZE_T g_PageTable[PAGE_TABLE_ENTRIES] NO_DATA(PAGE_TABLE_SIZE);


//========
// Common
//========

VOID Memory::Enable()
{
SIZE_T page_table=(SIZE_T)g_PageTable;
__asm volatile("msr ttbr0_el1, %0":: "r" (page_table));
constexpr SIZE_T MAIR_ATT_CACHED=0xFF;
constexpr SIZE_T MAIR_ATT_UNCACHED=0x44;
constexpr SIZE_T MAIR_ATT_DEVICE=0x04;
SIZE_T mair_el1=0;
mair_el1|=MAIR_ATT_CACHED<<(PAGE_ATT_CACHED*8);
mair_el1|=MAIR_ATT_UNCACHED<<(PAGE_ATT_UNCACHED*8);
mair_el1|=MAIR_ATT_DEVICE<<(PAGE_ATT_DEVICE*8);
__asm volatile("msr mair_el1, %0":: "r" (mair_el1));
constexpr SIZE_T TCR_IPS_4GB=(0ULL<<32);
constexpr SIZE_T TCR_IPS_1TB=(2ULL<<32);
constexpr SIZE_T TCR_EPD1=(1<<23);
constexpr SIZE_T TCR_TG0_4KB=(0<<14);
constexpr SIZE_T TCR_TG0_64KB=(1<<14);
constexpr SIZE_T TCR_SH0_INNER=(3<<12);
constexpr SIZE_T TCR_ORGN0_WB_WA=(1<<10);
constexpr SIZE_T TCR_IRGN0_WB_WA=(1<<8);
constexpr SIZE_T TCR_T0SZ_128GB=(27<<0);
SIZE_T tcr_el1=TCR_IPS_1TB|TCR_EPD1|TCR_TG0_4KB|TCR_SH0_INNER|TCR_ORGN0_WB_WA|TCR_IRGN0_WB_WA|TCR_T0SZ_128GB;
__asm volatile("msr tcr_el1, %0":: "r" (tcr_el1));
__asm volatile("isb");
__asm volatile("tlbi vmalle1; dsb sy; isb;");
SIZE_T sctlr_el1;
__asm volatile("mrs %0, sctlr_el1": "=r" (sctlr_el1));
constexpr SIZE_T SCTLR_I=(1<<12);
constexpr SIZE_T SCTLR_C=(1<<2);
constexpr SIZE_T SCTLR_M=(1<<0);
Bits::Set(sctlr_el1, SCTLR_I|SCTLR_C|SCTLR_M);
__asm volatile("msr sctlr_el1, %0":: "r" (sctlr_el1): "memory");
__asm volatile("isb; nop; nop; nop; nop"::: "memory");
}

VOID Memory::Initialize()
{
CreatePageTable();
Enable();
SIZE_T bss_start=(SIZE_T)&__bss_start;
SIZE_T bss_end=(SIZE_T)&__bss_end;
SIZE_T bss_size=bss_end-bss_start;
ZeroMemory(&__bss_start, bss_size);
SIZE_T heap_start=(SIZE_T)&__heap_start;
SIZE_T heap_end=UNCACHED_BASE;
SIZE_T heap_size=heap_end-heap_start;
g_heap=heap_create(heap_start, heap_size);
ARM_MEMORY info;
MailBox::GetArmMemory(&info);
heap_reserve(g_heap, info.Size, HIGH_MEM_BASE-info.Size);
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
	(*ctor)();
}


//================
// Common Private
//================

VOID Memory::CreatePageTable()
{
auto table=g_PageTable;
SIZE_T offset=0;
UINT page=0;
for(; offset<CACHED_END; offset+=REGION_SIZE)
	table[page++]=offset|ENTRY_CACHED;
for(; offset<UNCACHED_END; offset+=REGION_SIZE)
	table[page++]=(offset-UNCACHED_BASE)|ENTRY_UNCACHED;
for(; offset<AXI_IO_BASE; offset+=REGION_SIZE)
	table[page++]=0;
for(; offset<SOC_IO_END; offset+=REGION_SIZE)
	table[page++]=offset|ENTRY_DEVICE;
for(; offset<PCIE_IO_BASE; offset+=REGION_SIZE)
	table[page++]=0;
for(; offset<PCIE_IO_END; offset+=REGION_SIZE)
	table[page++]=offset|ENTRY_DEVICE;
for(; page<PAGE_TABLE_ENTRIES; page++)
	table[page]=0;
Cpu::DataSyncBarrier();
}

}}
