//================
// MemoryHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===============
// Memory-Helper
//===============

class MemoryHelper
{
public:
	// Common
	static INT Compare(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size);
	static VOID Copy(VOID* Destination, VOID const* Source, SIZE_T Size);
	static VOID Copy(SIZE_T* Destination, SIZE_T const* Source, SIZE_T Count);
	static VOID Copy(SIZE_T* Destination, SIZE_T* End, SIZE_T const* Source);
	static VOID Fill(VOID* Destination, SIZE_T Size, BYTE Value);
	static VOID Fill(SIZE_T* Destination, SIZE_T Count, SIZE_T Value);
	static VOID Fill(SIZE_T* Destination, SIZE_T* End, SIZE_T Value);
	static VOID Move(VOID* Destination, VOID const* Source, SIZE_T Size);
};
