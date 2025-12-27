//============
// string.cpp
//============

#include "Platform.h"


//=======
// Using
//=======

#include "MemoryHelper.h"


//========
// Memory
//========

//extern "C" INT memcmp(VOID const* buf1, VOID const* buf2, SIZE_T size)
//{
//return MemoryHelper::Compare(buf1, buf2, size);
//}
//
//extern "C" VOID* memcpy(VOID* dst, VOID const* src, SIZE_T size)
//{
//MemoryHelper::Copy(dst, src, size);
//return (VOID*)((SIZE_T)dst+size);
//}
//
//extern "C" VOID* memmove(VOID* dst, VOID const* src, SIZE_T size)
//{
//MemoryHelper::Move(dst, src, size);
//return (VOID*)((SIZE_T)dst+size);
//}
//
extern "C" VOID* memset(VOID* dst, INT value, SIZE_T size)
{
MemoryHelper::Fill(dst, size, (BYTE)value);
return (VOID*)((SIZE_T)dst+size);
}
