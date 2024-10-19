//============
// stdlib.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include <stdarg.h>
#include <stdio.h>
#include "Concurrency/TaskLock.h"
#include "heap.h"

using namespace Concurrency;


//========
// cstdio
//========

_reent* _impure_ptr=nullptr;

extern "C" INT fputc(INT c, FILE* file)
{
return 0;
}

extern "C" INT fputs(LPCSTR str, FILE* file)
{
return 0;
}

extern "C" size_t fwrite(VOID const* buf, size_t size, size_t count, FILE* file)
{
return 0;
}

extern "C" int sprintf(LPSTR str, LPCSTR format, ...)
{
return 0;
}

extern "C" size_t write(FILE* file, void const* buf, size_t size)
{
return 0;
}


//=========
// cstdlib
//=========

heap_handle_t g_heap=nullptr;
Mutex g_heap_mutex;

extern "C" LPCSTR getenv(LPCSTR name)
{
return nullptr;
}

extern "C" VOID* malloc(SIZE_T size)
{
TaskLock lock(g_heap_mutex);
return heap_alloc(g_heap, size);
}

extern "C" VOID free(VOID* buf)
{
TaskLock lock(g_heap_mutex);
heap_free(g_heap, buf);
}

extern "C" VOID* realloc(VOID* buf, SIZE_T size)
{
TaskLock lock(g_heap_mutex);
VOID* dst=heap_alloc(g_heap, size);
if(buf)
	{
	heap_block_info_t info;
	heap_block_get_info(g_heap, buf, &info);
	lock.Unlock();
	SIZE_T copy=Min(size, info.size);
	CopyMemory(dst, buf, copy);
	lock.Lock();
	heap_free(g_heap, buf);
	}
return dst;
}


//=========
// cstring
//=========

extern "C" INT memcmp(VOID const* buf1, VOID const* buf2, SIZE_T size)
{
LPCSTR buf1_ptr=(LPCSTR)buf1;
LPCSTR buf2_ptr=(LPCSTR)buf2;
for(SIZE_T u=0; u<size; u++)
	{
	if(buf1_ptr[u]>buf2_ptr[u])
		return 1;
	if(buf1_ptr[u]<buf2_ptr[u])
		return -1;
	}
return 0;
}

extern "C" VOID* memcpy(VOID* dst, VOID const* src, SIZE_T size)
{
LPSTR dst_buf=(LPSTR)dst;
LPCSTR src_buf=(LPCSTR)src;
for(SIZE_T u=0; u<size; u++)
	dst_buf[u]=src_buf[u];
return dst;
}

extern "C" VOID* memset(VOID* dst, INT value, SIZE_T size)
{
LPSTR dst_buf=(LPSTR)dst;
for(SIZE_T u=0; u<size; u++)
	dst_buf[u]=(CHAR)value;
return dst;
}

extern "C" LPCSTR strchr(LPCSTR str, INT c)
{
for(UINT pos=0; str[pos]; pos++)
	{
	if(str[pos]==c)
		return &str[pos];
	}
return nullptr;
}

extern "C" INT strcmp(LPCSTR str1, LPCSTR str2)
{
for(UINT pos=0; str1[pos]||str2[pos]; pos++)
	{
	if(str1[pos]<str2[pos])
		return -1;
	if(str1[pos]>str2[pos])
		return 1;
	}
return 0;
}

extern "C" INT strlen(LPCSTR str)
{
UINT len=0;
while(str[len])
	len++;
return len;
}

extern "C" INT strncmp(LPCSTR str1, LPCSTR str2, UINT len)
{
for(UINT pos=0; pos<len; pos++)
	{
	if(str1[pos]<str2[pos])
		return -1;
	if(str1[pos]>str2[pos])
		return 1;
	}
return 0;
}

extern "C" UINT strtoul(LPCSTR str, LPCSTR end, INT base)
{
UINT len=(UINT)(end-str);
UINT value=0;
StringScanUInt(str, &value, base, len);
return value;
}


//========
// cxxabi
//========

extern "C" void __cxa_atexit()
{}


//========
// stdlib
//========

extern "C" [[noreturn]] VOID abort()
{
Abort();
}

extern "C" [[noreturn]] VOID __assert_func(LPCSTR file, INT line, LPCSTR func, LPCSTR expr)
{
Abort(file, line, func, expr);
}
