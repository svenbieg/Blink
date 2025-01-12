//=========
// new.cpp
//=========

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include <new>
#include "Concurrency/TaskLock.h"

using namespace Concurrency;

extern heap_t* g_heap;
extern Mutex g_heap_mutex;


//=====
// new
//=====

void* operator new(__size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new(__size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void* operator new[](__size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](__size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void operator delete(void* buf)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete(void* buf, __size_t)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete[](void* array)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, array);
}
