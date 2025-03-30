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

using namespace Concurrency;


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

void abort()
{
throw AbortException();
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