//============
// stdlib.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include <new>
#include <stdlib.h>
#include <string.h>
#include "Concurrency/TaskLock.h"
#include "Devices/System/System.h"

using namespace Concurrency;

using System=Devices::System::System;


//=====
// new
//=====

heap_handle_t g_heap=nullptr;
Mutex g_heap_mutex;

void* operator new(std::size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new(std::size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void* operator new[](std::size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](std::size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void operator delete(void* buf)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete(void* buf, std::size_t)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete[](void* array)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, array);
}


//==========
// stdlib.h
//==========

VOID* __dso_handle=nullptr;

extern "C" void abort()
{
throw AbortException();
}

extern "C" void free(void* buf)
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

extern "C" void* malloc(size_t size)
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}


//==========
// string.h
//==========

extern "C" int memcmp(void const* buf1_ptr, void const* buf2_ptr, size_t size)
{
auto buf1=(LPCSTR)buf1_ptr;
auto buf2=(LPCSTR)buf2_ptr;
for(SIZE_T u=0; u<size; u++)
	{
	if(buf1[u]>buf2[u])
		return 1;
	if(buf1[u]<buf2[u])
		return -1;
	}
return 0;
}

extern "C" void* memcpy(void* dst_ptr, void const* src_ptr, size_t size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=*src++;
return dst;
}

extern "C" void* memmove(void* dst_ptr, void const* src_ptr, size_t size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
if(dst==src)
	return dst+size;
if(dst>src)
	{
	dst+=size;
	src+=size;
	while(dst>=dst_ptr)
		*--dst=*--src;
	}
else
	{
	auto end=dst+size;
	while(dst<end)
		*dst++=*src++;
	}
return dst+size;
}

extern "C" void* memset(void* dst_ptr, int value, size_t size)
{
auto dst=(LPSTR)dst_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=(CHAR)value;
return dst;
}
