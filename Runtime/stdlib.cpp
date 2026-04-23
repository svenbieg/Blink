//============
// stdlib.cpp
//============

#include "Platform.h"


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"
#include "Concurrency/WriteLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/System.h"
#include "heap.h"

using namespace Concurrency;
using namespace Devices::System;


//=========
// Globals
//=========

VOID* __dso_handle=nullptr;

heap_t* g_heap=nullptr;
CriticalMutex g_heap_mutex;


//========
// Common
//========

extern "C" VOID __assert_func(LPCSTR file, INT line, LPCSTR func, LPCSTR expr)
{
Cpu::Breakpoint();
System::Restart();
}

extern "C" VOID abort()
{
Cpu::Breakpoint();
System::Restart();
}

VOID* Allocate(SIZE_T size)
{
WriteLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

VOID Free(VOID* buf)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}
