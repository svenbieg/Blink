//============
// Memory.cpp
//============

#include "Memory.h"


//=======
// Using
//=======

#include "Concurrency/ReadLock.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/TaskMonitor.h"
#include "Concurrency/WriteLock.h"
#include "Devices/Peripherals.h"
#include "MemoryHelper.h"
#include <config.h>
#include <heap.h>

using namespace Concurrency;
using namespace Runtime;

extern COPY_T __bss_start[];
extern COPY_T __bss_end[];

extern BYTE __heap_start[];

typedef VOID (*CTOR_PTR)();

extern CTOR_PTR __init_array_start[];
extern CTOR_PTR __init_array_end[];

extern SIZE_T __page_table_start[];


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//=====
// MMU
//=====

constexpr UINT PAGE_ATT_CACHED=0;
constexpr UINT PAGE_ATT_UNCACHED=1;
constexpr UINT PAGE_ATT_DEVICE=2;


//============
// Page-Table
//============

constexpr UINT64 PAGE_TABLE_ENTRIES		=512;
constexpr UINT64 REGION_SIZE			=0x40000000; // 0x40000000 (1GB)

constexpr UINT64 ENTRY_PXN				=(1ULL<<53);
constexpr UINT64 ENTRY_UXN				=(1ULL<<54);
constexpr UINT64 ENTRY_ACC				=(1<<10);
constexpr UINT64 ENTRY_SHARE_OUT		=(2<<8);
constexpr UINT64 ENTRY_SHARE_IN			=(3<<8);
constexpr UINT64 ENTRY_ATT_CACHED		=(PAGE_ATT_CACHED<<2);
constexpr UINT64 ENTRY_ATT_UNCACHED		=(PAGE_ATT_UNCACHED<<2);
constexpr UINT64 ENTRY_ATT_DEVICE		=(PAGE_ATT_DEVICE<<2);
constexpr UINT64 ENTRY_TYPE_BLOCK		=1;

constexpr UINT64 ENTRY_CACHED			=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_CACHED|ENTRY_TYPE_BLOCK);
constexpr UINT64 ENTRY_UNCACHED			=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_UNCACHED|ENTRY_TYPE_BLOCK);
constexpr UINT64 ENTRY_DEVICE			=(ENTRY_PXN|ENTRY_UXN|ENTRY_ACC|ENTRY_SHARE_OUT|ENTRY_ATT_DEVICE|ENTRY_TYPE_BLOCK);


//========
// Common
//========

VOID* Memory::Allocate(SIZE_T size)
{
WriteLock lock(s_Mutex);
VOID* buf=heap_alloc((heap_t*)s_Heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

VOID Memory::Free(VOID* buf)
{
WriteLock lock(s_Mutex);
heap_free((heap_t*)s_Heap, buf);
}

VOID Memory::GetInfo(MEMORY_INFO* info)
{
ReadLock lock(s_Mutex);
auto heap=(heap_t*)s_Heap;
SIZE_T free=heap->free;
SIZE_T size=heap->size;
info->Available=free;
info->Total=size;
}

VOID Memory::Initialize()
{
CreatePageTable();
EnableMmu();
MemoryHelper::ZeroT(__bss_start, __bss_end);
SIZE_T heap_start=(SIZE_T)__heap_start;
SIZE_T heap_end=CONFIG_RAM_SIZE;
SIZE_T heap_size=heap_end-heap_start;
heap_t* heap=heap_create(heap_start, heap_size);
heap_reserve(heap, LOW_IO_BASE, LOW_IO_SIZE);
s_Heap=heap;
for(CTOR_PTR* ctor=__init_array_start; ctor!=__init_array_end; ctor++)
	(*ctor)();
}

VOID Memory::InitializeSecondary()
{
EnableMmu();
}

VOID* Memory::Uncached(VOID* buf)
{
return (VOID*)((SIZE_T)buf+UNCACHED_BASE);
}


//================
// Common Private
//================

VOID Memory::CreatePageTable()
{
auto table=(SIZE_T*)__page_table_start;
constexpr SIZE_T cached_from=CACHED_BASE/REGION_SIZE;
constexpr SIZE_T cached_to=CACHED_END/REGION_SIZE;
SIZE_T offset=0;
for(SIZE_T u=cached_from; u<cached_to; u++, offset+=REGION_SIZE)
	table[u]=offset|ENTRY_CACHED;
constexpr SIZE_T uncached_from=UNCACHED_BASE/REGION_SIZE;
constexpr SIZE_T uncached_to=UNCACHED_END/REGION_SIZE;
offset=0;
for(SIZE_T u=uncached_from; u<uncached_to; u++, offset+=REGION_SIZE)
	table[u]=offset|ENTRY_UNCACHED;
constexpr SIZE_T device_from=DEVICE_BASE/REGION_SIZE;
constexpr SIZE_T device_to=DEVICE_END/REGION_SIZE;
offset=DEVICE_BASE;
for(SIZE_T u=device_from; u<device_to; u++, offset+=REGION_SIZE)
	table[u]=offset|ENTRY_DEVICE;
constexpr SIZE_T zero_from=device_to;
constexpr SIZE_T zero_to=PAGE_TABLE_ENTRIES;
for(SIZE_T u=zero_from; u<zero_to; u++)
	table[u]=0;
}

VOID Memory::EnableMmu()
{
register UINT64 x0 asm("x0")=(UINT64)__page_table_start;
__asm volatile("msr TTBR0_EL1, x0":: "r" (x0));
constexpr UINT64 MAIR_ATT_CACHED=(0xFF<<(PAGE_ATT_CACHED*8));
constexpr UINT64 MAIR_ATT_UNCACHED=(0x44<<(PAGE_ATT_UNCACHED*8));
constexpr UINT64 MAIR_ATT_DEVICE=(0x04<<(PAGE_ATT_DEVICE*8));
x0=(MAIR_ATT_CACHED|MAIR_ATT_UNCACHED|MAIR_ATT_DEVICE);
__asm volatile("msr MAIR_EL1, x0":: "r" (x0));
constexpr UINT64 TCR_IPS_4GB=(0ULL<<32);
constexpr UINT64 TCR_IPS_1TB=(2ULL<<32);
constexpr UINT64 TCR_EPD1=(1<<23);
constexpr UINT64 TCR_TG0_4KB=(0<<14);
constexpr UINT64 TCR_TG0_64KB=(1<<14);
constexpr UINT64 TCR_SH0_INNER=(3<<12);
constexpr UINT64 TCR_ORGN0_WB_WA=(1<<10);
constexpr UINT64 TCR_IRGN0_WB_WA=(1<<8);
constexpr UINT64 TCR_T0SZ_128GB=(27<<0);
x0=(TCR_IPS_1TB|TCR_EPD1|TCR_TG0_4KB|TCR_SH0_INNER|TCR_ORGN0_WB_WA|TCR_IRGN0_WB_WA|TCR_T0SZ_128GB);
__asm volatile("msr TCR_EL1, x0":: "r" (x0));
Cpu::InstructionSyncBarrier();
__asm volatile("tlbi vmalle1");
Cpu::DataSyncBarrier();
Cpu::InstructionSyncBarrier();
constexpr UINT64 SCTLR_I=(1<<12);
constexpr UINT64 SCTLR_C=(1<<2);
constexpr UINT64 SCTLR_M=(1<<0);
x0=(SCTLR_I|SCTLR_C|SCTLR_M);
__asm volatile("\
mrs x1, SCTLR_EL1\n\
orr x1, x1, x0\n\
msr SCTLR_EL1, x1\n\
isb\n\
":: "r" (x0): "x1");
}

VOID* Memory::s_Heap=nullptr;
ServiceMutex Memory::s_Mutex;

}}