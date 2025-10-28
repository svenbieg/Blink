//==================
// MemoryHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "MemoryHelper.h"


//========
// Common
//========

INT MemoryHelper::Compare(VOID const* buf1_ptr, VOID const* buf2_ptr, SIZE_T size)
{
auto buf1=(BYTE const*)buf1_ptr;
auto buf2=(BYTE const*)buf2_ptr;
for(SIZE_T u=0; u<size; u++)
	{
	if(buf1[u]>buf2[u])
		return 1;
	if(buf1[u]<buf2[u])
		return -1;
	}
return 0;
}

INT MemoryHelper::Compare32(UINT const* buf1, UINT const* buf2, SIZE_T count)
{
for(SIZE_T u=0; u<count; u++)
	{
	if(buf1[u]>buf2[u])
		return 1;
	if(buf1[u]<buf2[u])
		return -1;
	}
return 0;
}

INT MemoryHelper::Compare64(UINT64 const* buf1, UINT64 const* buf2, SIZE_T count)
{
for(SIZE_T u=0; u<count; u++)
	{
	if(buf1[u]>buf2[u])
		return 1;
	if(buf1[u]<buf2[u])
		return -1;
	}
return 0;
}

VOID MemoryHelper::Copy(VOID* dst_ptr, VOID const* src_ptr, SIZE_T size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=*src++;
}

VOID MemoryHelper::Copy32(UINT* dst, UINT const* src, SIZE_T count)
{
while(count--)
	*dst++=*src++;
}

VOID MemoryHelper::Copy32(UINT* dst, UINT const* src, UINT* end)
{
while(dst<end)
	*dst++=*src++;
}

VOID MemoryHelper::Copy64(UINT64* dst, UINT64 const* src, SIZE_T count)
{
while(count--)
	*dst++=*src++;
}

VOID MemoryHelper::Copy64(UINT64* dst, UINT64 const* src, UINT64* end)
{
while(dst<end)
	*dst++=*src++;
}

VOID MemoryHelper::Fill(VOID* dst_ptr, SIZE_T size, BYTE value)
{
auto dst=(BYTE*)dst_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=value;
}

VOID MemoryHelper::Fill32(UINT* dst, SIZE_T count, UINT value)
{
while(count--)
	*dst++=value;
}

VOID MemoryHelper::Fill32(UINT* dst, UINT* end, UINT value)
{
while(dst<end)
	*dst++=value;
}

VOID MemoryHelper::Fill64(UINT64* dst, SIZE_T count, UINT64 value)
{
while(count--)
	*dst++=value;
}

VOID MemoryHelper::Fill64(UINT64* dst, UINT64* end, UINT64 value)
{
while(dst<end)
	*dst++=value;
}

VOID MemoryHelper::Move(VOID* dst_ptr, VOID const* src_ptr, SIZE_T size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
if(dst==src)
	return;
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
}

VOID MemoryHelper::Move32(UINT* dst_ptr, UINT const* src, SIZE_T count)
{
auto dst=dst_ptr;
if(dst==src)
	return;
if(dst>src)
	{
	dst+=count;
	src+=count;
	while(dst>=dst_ptr)
		*--dst=*--src;
	}
else
	{
	auto end=dst+count;
	while(dst<end)
		*dst++=*src++;
	}
}

VOID MemoryHelper::Move64(UINT64* dst_ptr, UINT64 const* src, SIZE_T count)
{
auto dst=dst_ptr;
if(dst==src)
	return;
if(dst>src)
	{
	dst+=count;
	src+=count;
	while(dst>=dst_ptr)
		*--dst=*--src;
	}
else
	{
	auto end=dst+count;
	while(dst<end)
		*dst++=*src++;
	}
}
