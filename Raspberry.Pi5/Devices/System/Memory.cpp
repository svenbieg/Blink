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


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


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
constexpr UINT64 ENTRY_ATT_CACHED		=(MMU_ATT_CACHED<<2);
constexpr UINT64 ENTRY_ATT_UNCACHED		=(MMU_ATT_UNCACHED<<2);
constexpr UINT64 ENTRY_ATT_DEVICE		=(MMU_ATT_DEVICE<<2);
constexpr UINT64 ENTRY_TYPE_BLOCK		=1;

constexpr UINT64 ENTRY_CACHED			=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_CACHED|ENTRY_TYPE_BLOCK);
constexpr UINT64 ENTRY_UNCACHED			=(ENTRY_ACC|ENTRY_SHARE_IN|ENTRY_ATT_UNCACHED|ENTRY_TYPE_BLOCK);
constexpr UINT64 ENTRY_DEVICE			=(ENTRY_PXN|ENTRY_UXN|ENTRY_ACC|ENTRY_SHARE_OUT|ENTRY_ATT_DEVICE|ENTRY_TYPE_BLOCK);

struct PAGE_TABLE
{
UINT64 Entry[512];
};

consteval PAGE_TABLE CreatePageTable()
{
PAGE_TABLE table;
constexpr SIZE_T cached_from=CACHED_BASE/REGION_SIZE;
constexpr SIZE_T cached_to=CACHED_END/REGION_SIZE;
SIZE_T offset=0;
for(SIZE_T u=cached_from; u<cached_to; u++, offset+=REGION_SIZE)
	table.Entry[u]=offset|ENTRY_CACHED;
constexpr SIZE_T uncached_from=UNCACHED_BASE/REGION_SIZE;
constexpr SIZE_T uncached_to=UNCACHED_END/REGION_SIZE;
offset=0;
for(SIZE_T u=uncached_from; u<uncached_to; u++, offset+=REGION_SIZE)
	table.Entry[u]=offset|ENTRY_UNCACHED;
constexpr SIZE_T device_from=DEVICE_BASE/REGION_SIZE;
constexpr SIZE_T device_to=DEVICE_END/REGION_SIZE;
offset=DEVICE_BASE;
for(SIZE_T u=device_from; u<device_to; u++, offset+=REGION_SIZE)
	table.Entry[u]=offset|ENTRY_DEVICE;
constexpr SIZE_T zero_from=device_to;
constexpr SIZE_T zero_to=TypeHelper::ArraySize(table.Entry);
for(SIZE_T u=zero_from; u<zero_to; u++)
	table.Entry[u]=0;
return table;
}

[[gnu::used, gnu::section(".page_table")]]
const PAGE_TABLE g_PageTable=CreatePageTable();


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

VOID* Memory::Uncached(VOID* buf)
{
return (VOID*)((SIZE_T)buf+UNCACHED_BASE);
}


//================
// Common Private
//================

VOID* Memory::s_Heap=nullptr;
ServiceMutex Memory::s_Mutex;

}}