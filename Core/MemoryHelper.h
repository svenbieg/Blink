//================
// MemoryHelper.h
//================

#pragma once


//========
// Macros
//========

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))
#define SECTION(x) __attribute__((section(x)))


//========
// Common
//========

INT CompareMemory(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size);
VOID CopyMemory(VOID* Destination, VOID const* Source, SIZE_T Size);
VOID FillMemory(VOID* Destination, SIZE_T Size, UINT Value);
VOID MoveMemory(VOID* Destination, VOID const* Source, SIZE_T Size);
VOID ZeroMemory(VOID* Destination, SIZE_T Size);
