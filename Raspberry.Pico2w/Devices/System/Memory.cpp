//============
// Memory.cpp
//============

#include "Memory.h"


//=======
// Using
//=======

#include "Concurrency/ReadLock.h"
#include "Concurrency/SpinLock.h"
#include "Concurrency/WriteLock.h"
#include "Devices/Peripherals.h"
#include "Runtime/Configuration.h"
#include "MemoryHelper.h"
#include <heap.h>

using namespace Concurrency;

extern COPY_T __flash_data_start;
extern COPY_T __data_start;
extern COPY_T __data_end;

extern COPY_T __bss_start;
extern COPY_T __bss_end;

extern SIZE_T __heap_start;

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

VOID Memory::Free(VOID* buf)noexcept
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
MemoryHelper::CopyT(&__data_start, &__flash_data_start, &__data_end);
MemoryHelper::ZeroT(&__bss_start, &__bss_end);
SIZE_T heap_start=(SIZE_T)&__heap_start;
SIZE_T heap_end=SRAM_END;
SIZE_T heap_size=heap_end-heap_start;
s_Heap=heap_create(heap_start, heap_size);
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
	(*ctor)();
}


//================
// Common Private
//================

VOID* Memory::s_Heap=nullptr;
CriticalMutex Memory::s_Mutex;

}}