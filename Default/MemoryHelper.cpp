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

VOID MemoryHelper::Copy(VOID* dst_ptr, VOID const* src_ptr, SIZE_T size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=*src++;
}

VOID MemoryHelper::Copy(SIZE_T* dst, SIZE_T const* src, SIZE_T count)
{
while(count--)
	*dst++=*src++;
}

VOID MemoryHelper::Copy(SIZE_T* dst, SIZE_T* end, SIZE_T const* src)
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

VOID MemoryHelper::Fill(SIZE_T* dst, SIZE_T count, SIZE_T value)
{
while(count--)
	*dst++=value;
}

VOID MemoryHelper::Fill(SIZE_T* dst, SIZE_T* end, SIZE_T value)
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
