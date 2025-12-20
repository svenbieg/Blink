//=============
// BitHelper.h
//=============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//============
// Bit-Helper
//============

class BitHelper
{
public:
	// Common
	static inline VOID Clear(volatile UINT& Operand)
		{
		Operand=0;
		}
	static inline VOID Clear(volatile UINT& Operand, UINT Mask)
		{
		Operand&=~Mask;
		}
	static inline VOID Clear(volatile UINT64& Operand, UINT64 Mask)
		{
		Operand&=~Mask;
		}
	static inline BYTE Get(volatile BYTE& Operand)
		{
		return Operand;
		}
	static inline WORD Get(volatile WORD& Operand)
		{
		return Operand;
		}
	static inline UINT Get(volatile UINT& Operand)
		{
		return Operand;
		}
	static inline UINT Get(volatile UINT& Operand, UINT Mask)
		{
		return Operand&Mask;
		}
	static inline UINT Get(volatile UINT64& Operand, UINT Mask)
		{
		return ((UINT)Operand)&Mask;
		}
	static inline UINT64 Get(volatile UINT64& Operand, UINT64 Mask)
		{
		return Operand&Mask;
		}
	static inline VOID Set(volatile UINT& Operand, UINT Mask)
		{
		Operand|=Mask;
		}
	static inline VOID Set(volatile UINT64& Operand, UINT64 Mask)
		{
		Operand|=Mask;
		}
	static inline VOID Set(volatile UINT& Operand, UINT Mask, UINT Value)
		{
		UINT value=Operand;
		value&=~Mask;
		value|=Value;
		Operand=value;
		}
	static inline VOID Set(volatile UINT64& Operand, UINT64 Mask, UINT64 Value)
		{
		UINT64 value=Operand;
		value&=~Mask;
		value|=Value;
		Operand=value;
		}
	static inline VOID Switch(volatile UINT& Operand, UINT Mask)
		{
		Operand^=Mask;
		}
};
