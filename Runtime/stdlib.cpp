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
#include "UI/Console.h"
#include "Exception.h"
#include "heap.h"

using namespace Concurrency;
using namespace UI;


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
Console::Print("%s (%i) - func: assert(%s)\n", file, line, func, expr);
throw AbortException();
}

extern "C" VOID abort()
{
throw AbortException();
}

VOID* Allocate(SIZE_T size)
{
WriteLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

VOID Free(VOID* buf)
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}
