//==================
// MemoryHelper.cpp
//==================

#include "pch.h"


//========
// Common
//========

INT CompareMemory(VOID const* buf1_ptr, VOID const* buf2_ptr, SIZE_T size)
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

VOID CopyMemory(VOID* dst_ptr, VOID const* src_ptr, SIZE_T size)
{
auto dst=(LPSTR)dst_ptr;
auto src=(LPCSTR)src_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=*src++;
}

VOID FillMemory(VOID* dst_ptr, SIZE_T size, UINT value)
{
auto dst=(LPSTR)dst_ptr;
auto end=dst+size;
while(dst<end)
	*dst++=(CHAR)value;
}

VOID MoveMemory(VOID* dst_ptr, VOID const* src_ptr, SIZE_T size)
{
if(dst_ptr==src_ptr)
	return;
if(dst_ptr>src_ptr)
	{
	auto dst=(LPSTR)dst_ptr+size;
	auto src=(LPCSTR)src_ptr+size;
	while(dst>=dst_ptr)
		*--dst=*--src;
	}
else
	{
	auto dst=(LPSTR)dst_ptr;
	auto src=(LPCSTR)src_ptr;
	auto end=dst+size;
	while(dst<end)
		*dst++=*src++;
	}
}

VOID ZeroMemory(VOID* dst_ptr, SIZE_T size)
{
auto dst=(LPSTR)dst_ptr;
for(SIZE_T u=0; u<size; u++)
	dst[u]=0;
}
