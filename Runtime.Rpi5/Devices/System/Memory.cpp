//============
// Memory.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <base.h>
#include <config.h>
#include <heap.h>
#include "Devices/System/Cpu.h"
#include "Devices/System/Memory.h"

using namespace Devices::System;

extern COPY_T __bss_start;
extern COPY_T __bss_end;

extern BYTE __heap_start;

extern heap_t* g_heap;

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

VOID Memory::Initialize()
{
MemoryHelper::ZeroT(&__bss_start, &__bss_end);
SIZE_T heap_start=(SIZE_T)&__heap_start;
SIZE_T heap_end=RAM_SIZE;
SIZE_T heap_size=heap_end-heap_start;
g_heap=heap_create(heap_start, heap_size);
heap_reserve(g_heap, LOW_IO_BASE, LOW_IO_SIZE);
for(CTOR_PTR* ctor=&__init_array_start; ctor!=&__init_array_end; ctor++)
	(*ctor)();
}

VOID* Memory::Uncached(VOID* buf)
{
return (VOID*)((SIZE_T)buf+UNCACHED_BASE);
}

}}