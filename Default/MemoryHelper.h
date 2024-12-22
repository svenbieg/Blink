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
	template <class _size_t>
	static inline _size_t AlignDown(_size_t Value, UINT Align)
		{
		return Value&~(Align-1);
		}
	template <class _size_t>
	static inline _size_t AlignUp(_size_t Value, UINT Align)
		{
		return Value+(Align-Value%Align)%Align;
		}
	static INT Compare(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size);
	static VOID Copy(VOID* Destination, VOID const* Source, SIZE_T Size);
	static VOID Fill(VOID* Destination, SIZE_T Size, UINT Value);
	static VOID Move(VOID* Destination, VOID const* Source, SIZE_T Size);
};
