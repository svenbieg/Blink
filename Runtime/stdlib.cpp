//============
// stdlib.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include <stdlib.h>
#include "Concurrency/TaskLock.h"

using namespace Concurrency;


//=========
// Globals
//=========

VOID* __dso_handle=nullptr;

heap_t* g_heap=nullptr;
Mutex g_heap_mutex;


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
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void* malloc(size_t size)
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

}