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
	static VOID Copy32(UINT* Destination, UINT const* Source, SIZE_T Count);
	static VOID Copy32(UINT* Destination, UINT const* Source, UINT* End);
	static VOID Copy64(UINT64* Destination, UINT64 const* Source, SIZE_T Count);
	static VOID Copy64(UINT64* Destination, UINT64 const* Source, UINT64* End);
	static VOID Fill(VOID* Destination, SIZE_T Size, BYTE Value);
	static VOID Fill32(UINT* Destination, SIZE_T Count, UINT Value);
	static VOID Fill32(UINT* Destination, UINT* End, UINT Value);
	static VOID Fill64(UINT64* Destination, SIZE_T Count, UINT64 Value);
	static VOID Fill64(UINT64* Destination, UINT64* End, UINT64 Value);
	static VOID Move(VOID* Destination, VOID const* Source, SIZE_T Size);
	static VOID Move32(UINT* Destination, UINT const* Source, SIZE_T Count);
	static VOID Move64(UINT64* Destination, UINT64 const* Source, SIZE_T Size);
	static inline VOID Zero(VOID* Destination, SIZE_T Size) { Fill(Destination, Size, 0); }
	static inline VOID Zero32(UINT* Destination, SIZE_T Count) { Fill32(Destination, Count, 0); }
	static inline VOID Zero32(UINT* Destination, UINT* End) { Fill32(Destination, End, 0); }
	static inline VOID Zero64(UINT64* Destination, SIZE_T Count) { Fill64(Destination, Count, 0); }
	static inline VOID Zero64(UINT64* Destination, UINT64* End) { Fill64(Destination, End, 0); }
};
