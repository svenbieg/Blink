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
	static VOID Copy(UINT* Destination, UINT const* Source, SIZE_T Count);
	static VOID Copy(UINT* Destination, UINT* End, UINT const* Source);
	static VOID Copy(UINT64* Destination, UINT64 const* Source, SIZE_T Count);
	static VOID Copy(UINT64* Destination, UINT64* End, UINT64 const* Source);
	static VOID Fill(VOID* Destination, SIZE_T Size, BYTE Value);
	static VOID Fill(UINT* Destination, SIZE_T Count, UINT Value);
	static VOID Fill(UINT* Destination, UINT* End, UINT Value);
	static VOID Fill(UINT64* Destination, SIZE_T Count, UINT64 Value);
	static VOID Fill(UINT64* Destination, UINT64* End, UINT64 Value);
	static VOID Move(VOID* Destination, VOID const* Source, SIZE_T Size);
	static inline VOID Zero(VOID* Destination, SIZE_T Size) { Fill(Destination, Size, 0); }
	static inline VOID Zero(UINT* Destination, SIZE_T Count) { Fill(Destination, Count, 0); }
	static inline VOID Zero(UINT* Destination, UINT* End) { Fill(Destination, End, 0); }
	static inline VOID Zero(UINT64* Destination, SIZE_T Count) { Fill(Destination, Count, 0); }
	static inline VOID Zero(UINT64* Destination, UINT64* End) { Fill(Destination, End, 0); }
};
