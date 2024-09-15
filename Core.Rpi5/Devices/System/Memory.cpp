//============
// Memory.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Peripherals.h"
#include "heap.h"
#include "Memory.h"
#include "Settings.h"

using namespace Devices::System;

extern BYTE __bss_start;
extern BYTE __bss_end;

extern BYTE __page_table_start;

extern BYTE __heap_start;

extern heap_handle_t g_heap;

typedef VOID (*CTOR_PTR)();

extern CTOR_PTR __init_array_start;
extern CTOR_PTR __init_array_end;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//==========
// Settings
//==========

constexpr SIZE_T RAM_SIZE=LOCAL_IO_BASE;


//============
// Page-Table
//============

constexpr UINT64 PAGE_TABLE_ENTRIES=512;
constexpr UINT64 REGION_SIZE=0x40000000;

constexpr UINT PAGE_ATT_NORMAL=0;
constexpr UINT PAGE_ATT_DEVICE=1;

constexpr UINT64 ENTRY_PXN=(1ULL<<53);
constexpr UINT64 ENTRY_UXN=(1ULL<<54);
constexpr UINT64 ENTRY_ACC=(1<<10);
constexpr UINT64 ENTRY_SHARE_OUT=(2<<8);
constexpr UINT64 ENTRY_SHARE_IN=(3<<8);
constexpr UINT64 ENTRY_ATT_NORMAL=(PAGE_ATT_NORMAL<<2);
constexpr UINT64 ENTRY_ATT_DEVICE=(PAGE_ATT_DEVICE<<2);
constexpr UINT64 ENTRY_TYPE_BLOCK=1;

constexpr UINT64 ENTRY_DEVICE=(ENTRY_PXN|ENTRY_UXN|ENTRY_ACC|ENTRY_SHARE_OUT|ENTRY_ATT_DEVICE|ENTRY_TYPE_BLOCK);
constexpr UINT64 ENTRY_MEMORY=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_NORMAL|ENTRY_TYPE_BLOCK);


//========
// Common
//========

VOID Memory::Enable()
{
UINT64 page_table=(UINT64)&__page_table_start;
__asm volatile("msr ttbr0_el1, %0":: "r" (page_table));
constexpr UINT64 MAIR_ATT_NORMAL=0xFF;
constexpr UINT64 MAIR_ATT_DEVICE=0x04;
UINT64 mair_el1=0;
mair_el1|=MAIR_ATT_NORMAL<<(PAGE_ATT_NORMAL*8);
mair_el1|=MAIR_ATT_DEVICE<<(PAGE_ATT_DEVICE*8);
__asm volatile("msr mair_el1, %0":: "r" (mair_el1));
constexpr UINT64 TCR_IPS_4GB=(0ULL<<32);
constexpr UINT64 TCR_IPS_1TB=(2ULL<<32);
constexpr UINT64 TCR_EPD1=(1<<23); // Disable Table walk from TTBR1
constexpr UINT64 TCR_TG0_4KB=(0<<14);
constexpr UINT64 TCR_TG0_64KB=(1<<14);
constexpr UINT64 TCR_SH0_INNER=(3<<12);
constexpr UINT64 TCR_ORGN0_WB_WA=(1<<10);
constexpr UINT64 TCR_IRGN0_WB_WA=(1<<8);
constexpr UINT64 TCR_T0SZ_128GB=(27<<0);
UINT64 tcr_el1=TCR_IPS_1TB|TCR_EPD1|TCR_TG0_4KB|TCR_SH0_INNER|TCR_ORGN0_WB_WA|TCR_IRGN0_WB_WA|TCR_T0SZ_128GB;
__asm volatile("msr tcr_el1, %0":: "r" (tcr_el1));
//__asm volatile("isb");
__asm volatile("tlbi vmalle1; dsb sy; isb;"); // Invalidate TLBs
UINT64 sctlr_el1;
__asm volatile("mrs %0, sctlr_el1": "=r" (sctlr_el1));
constexpr UINT64 SCTLR_I=(1<<12);
constexpr UINT64 SCTLR_C=(1<<2);
constexpr UINT64 SCTLR_M=(1<<0);
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
SIZE_T heap_end=RAM_SIZE;
SIZE_T heap_size=heap_end-heap_start;
g_heap=heap_create(heap_start, heap_size);
heap_reserve(g_heap, LOW_IO_BASE, LOW_IO_SIZE);
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
	(*ctor)();
}


//================
// Common Private
//================

VOID Memory::CreatePageTable()
{
UINT64* table=(UINT64*)&__page_table_start;
SIZE_T offset=0;
UINT page=0;
for(; offset<RAM_SIZE; offset+=REGION_SIZE)
	table[page++]=offset|ENTRY_MEMORY;
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
__asm volatile("dsb sy");
}

}}
