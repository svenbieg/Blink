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
#include "Runtime/Configuration.h"
#include "MemoryHelper.h"
#include <heap.h>

using namespace Concurrency;
using namespace Runtime;

extern COPY_T __bss_start;
extern COPY_T __bss_end;

extern BYTE __heap_start;

typedef VOID (*CTOR_PTR)();

extern CTOR_PTR __init_array_start;
extern CTOR_PTR __init_array_end;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


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
MemoryHelper::ZeroT(&__bss_start, &__bss_end);
SIZE_T heap_start=(SIZE_T)&__heap_start;
SIZE_T heap_end=CONFIG_RAM_SIZE;
SIZE_T heap_size=heap_end-heap_start;
heap_t* heap=heap_create(heap_start, heap_size);
heap_reserve(heap, LOW_IO_BASE, LOW_IO_SIZE);
s_Heap=heap;
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
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
CriticalMutex Memory::s_Mutex;

}}