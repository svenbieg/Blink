//================
// MemoryHelper.h
//================

#pragma once


//=======
// Using
//=======

#include <cstring>
#include "TypeHelper.h"


//==========
// Settings
//==========

constexpr SIZE_T PAGE_SIZE=4096;


//========
// Macros
//========

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))


//===========
// Alignment
//===========

template <class _size_t>
inline _size_t AlignDown(_size_t Value, UINT Align)
{
return Value&~(Align-1);
}

template <class _size_t>
inline _size_t AlignUp(_size_t Value, UINT Align)
{
return Value+(Align-Value%Align)%Align;
}


//========
// Common
//========

inline INT CompareMemory(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size)
{
return std::memcmp(Buffer1, Buffer2, Size);
}

inline VOID CopyMemory(VOID* Destination, VOID const* Source, SIZE_T Size)
{
std::memcpy(Destination, Source, Size);
}

inline VOID FillMemory(VOID* Destination, SIZE_T Size, UINT Value)
{
std::memset(Destination, Value, Size);
}

inline VOID MoveMemory(VOID* Destination, VOID const* Source, SIZE_T Size)
{
std::memmove(Destination, Source, Size);
}

inline VOID ZeroMemory(VOID* Destination, SIZE_T Size)
{
std::memset(Destination, 0, Size);
}
