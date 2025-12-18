//=========
// new.cpp
//=========

#include <heap.h>
#include <new>
#include "Concurrency/CriticalMutex.h"
#include "Concurrency/WriteLock.h"

using namespace Concurrency;

extern heap_t* g_heap;
extern CriticalMutex g_heap_mutex;


//=====
// new
//=====

void* operator new(__size_t size)
{
WriteLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](__size_t size)
{
WriteLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void operator delete(void* buf)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete(void* buf, __size_t)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete[](void* array)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, array);
}

void operator delete[](void* array, __size_t)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, array);
}
