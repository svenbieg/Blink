//============
// stdlib.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include <stdlib.h>
#include "Concurrency/CriticalMutex.h"
#include "Concurrency/WriteLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/System.h"

using namespace Concurrency;
using namespace Devices::System;


//=========
// Globals
//=========

VOID* __dso_handle=nullptr;

heap_t* g_heap=nullptr;
CriticalMutex g_heap_mutex;


//===========
// Namespace
//===========

extern "C" {


//========
// Common
//========

__iram void abort()
{
throw AbortException();
}

void* aligned_alloc(size_t align, size_t size)
{
WriteLock lock(g_heap_mutex);
return heap_alloc_aligned(g_heap, size, align);
}

void free(void* buf)
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void* malloc(size_t size)
{
WriteLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

}