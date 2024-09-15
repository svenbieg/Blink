//==================
// MemoryHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <cstring>
#include <string.h>


//========
// Common
//========

INT CompareMemory(VOID const* buf1, VOID const* buf2, SIZE_T size)
{
return memcmp(buf1, buf2, size);
}

VOID CopyMemory(VOID* dst, VOID const* src, SIZE_T size)
{
memcpy(dst, src, size);
}

VOID FillMemory(VOID* dst, SIZE_T size, UINT value)
{
memset(dst, value, size);
}

VOID MoveMemory(VOID* dst, VOID const* src, SIZE_T size)
{
memmove(dst, src, size);
}

VOID ZeroMemory(VOID* dst, SIZE_T size)
{
memset(dst, 0, size);
}
