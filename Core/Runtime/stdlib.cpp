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
throw NotImplementedException();
}

extern "C" INT fputs(LPCSTR str, FILE* file)
{
throw NotImplementedException();
}

extern "C" size_t fwrite(VOID const* buf, size_t size, size_t count, FILE* file)
{
throw NotImplementedException();
}

extern "C" int sprintf(LPSTR str, LPCSTR format, ...)
{
throw NotImplementedException();
}

extern "C" size_t write(FILE* file, void const* buf, size_t size)
{
throw NotImplementedException();
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
throw NotImplementedException();
return nullptr;
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
throw NotImplementedException();
}

extern "C" INT strcmp(LPCSTR str1, LPCSTR str2)
{
throw NotImplementedException();
}

extern "C" INT strlen(LPCSTR str)
{
throw NotImplementedException();
}

extern "C" INT strncmp(LPCSTR str1, LPCSTR str2, UINT len)
{
	throw NotImplementedException();
}

extern "C" UINT strtoul(LPCSTR str, LPCSTR end, INT base)
{
throw NotImplementedException();
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
