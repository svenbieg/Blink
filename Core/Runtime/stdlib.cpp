//============
// stdlib.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <heap.h>
#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Concurrency/TaskLock.h"
#include "Devices/System/System.h"

using namespace Concurrency;

using System=Devices::System::System;


//========
// cstdio
//========

_reent* _impure_ptr=nullptr;

extern "C" INT fputc(INT c, FILE* file)
{
return fwrite(&c, 1, 1, file);
}

extern "C" INT fputs(LPCSTR str, FILE* file)
{
INT len=0;
while(str[len])
	{
	size_t written=fwrite(&str[len], 1, 1, file);
	if(!written)
		break;
	len++;
	}
return len;
}

extern "C" size_t fwrite(VOID const* buf, size_t size, size_t count, FILE* file)
{
return 0;
}

/*int printf(LPCSTR format, ...)
{
va_list lst;
va_start(lst, format);
vfprintf(stdout, format, lst);
va_end(lst);
return 0;
}*/

/*int vfprintf(FILE* file, LPCSTR format, va_list args)
{
return 0;
}*/

//extern "C" size_t write(FILE* file, void const* buf, size_t size)
//{
//return fwrite(buf, 1, size, file);
//}


//========
// cxxabi
//========

extern "C" void __cxa_atexit()
{}


//=============
// exception.h
//=============

extern "C" void __assert_func(const char* file, int line, const char* func, const char* expr)
{
throw NotImplementedException();
}


//=====
// new
//=====

heap_handle_t g_heap=nullptr;
Mutex g_heap_mutex;

void* operator new(size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new(size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void* operator new[](size_t size)
{
TaskLock lock(g_heap_mutex);
auto buf=heap_alloc(g_heap, size);
if(!buf)
	throw OutOfMemoryException();
return buf;
}

void* operator new[](size_t size, std::nothrow_t const&)noexcept
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

void operator delete(void* buf)noexcept
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

void operator delete(void* buf, size_t)noexcept
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

extern "C" LPSTR getenv(LPCSTR name)
{
return nullptr;
}

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

extern "C" void* realloc(void* buf, size_t size)
{
throw NotImplementedException();
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

extern "C" int sprintf(LPSTR str, LPCSTR format, ...)
{
throw NotImplementedException();
}

extern "C" LPSTR strchr(LPCSTR str, INT c)
{
if(!str)
	return nullptr;
while(str[0]!=c)
	str++;
return const_cast<LPSTR>(str);
}

extern "C" INT strcmp(LPCSTR str1, LPCSTR str2)
{
if(!str1)
	{
	if(!str2)
		return 0;
	return -1;
	}
if(!str2)
	return 1;
for(UINT pos=0; pos>=0; pos++)
	{
	if(str1[pos]>str2[pos])
		return 1;
	if(str1[pos]<str2[pos])
		return -1;
	if(!str1[pos])
		break;
	}
return 0;
}

extern "C" size_t strlen(LPCSTR str)
{
if(!str)
	return 0;
UINT len=0;
for(; str[len]; len++);
return len;
}

extern "C" int strncmp(LPCSTR str1, LPCSTR str2, size_t len)
{
if(!str1)
	{
	if(!str2)
		return 0;
	return -1;
	}
if(!str2)
	return 1;
for(UINT pos=0; pos<len; pos++)
	{
	if(str1[pos]>str2[pos])
		return 1;
	if(str1[pos]<str2[pos])
		return -1;
	if(!str1[pos])
		break;
	}
return 0;
}

extern "C" UINT strtoul(LPCSTR str, LPSTR* end_ptr, INT base)
{
UINT len=0;
LPSTR end=*end_ptr;
if(end>str)
	len=(UINT)(end-str);
UINT value=0;
UINT read=StringHelper::ScanUInt(str, &value, base, len);
*end_ptr=const_cast<LPSTR>(str)+read;
return value;
}
