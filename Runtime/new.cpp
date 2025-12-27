//=========
// new.cpp
//=========

#include <new>


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"
#include "Concurrency/WriteLock.h"
#include "Exception.h"
#include <heap.h>

using namespace Concurrency;

extern heap_t* g_heap;
extern CriticalMutex g_heap_mutex;


//=====
// new
//=====

void* operator new(__ISENSE_T size)
{
WriteLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](__ISENSE_T size)
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

void operator delete(void* buf, __ISENSE_T)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete[](void* array)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, array);
}

void operator delete[](void* array, __ISENSE_T)noexcept
{
WriteLock lock(g_heap_mutex);
heap_free(g_heap, array);
}
