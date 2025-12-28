//=============
// BitHelper.h
//=============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"
#include <assert.h>


//===========
// Bitfields
//===========

typedef struct
{
WORD Mask;
BYTE Shift;
}BITS8;

typedef struct
{
WORD Mask;
BYTE Shift;
}BITS16;

typedef struct
{
UINT Mask;
BYTE Shift;
}BITS32;

typedef struct
{
UINT64 Mask;
BYTE Shift;
}BITS64;

typedef BITS32 BITS;


//============
// Bit-Helper
//============

class BitHelper
{
public:
	// Common
	template <class _value_t> static UINT BitCount(_value_t Value)
		{
		UINT bits=0;
		while(Value)
			{
			Value>>=1;
			bits++;
			}
		return bits;
		}
	static inline VOID Clear(UINT& Operand) { Operand=0; }
	static inline VOID Clear(UINT& Operand, UINT Mask) { Operand&=~Mask; }
	static inline VOID Clear(UINT64& Operand, UINT64 Mask) { Operand&=~Mask; }
	static inline BYTE Get(BYTE Operand, BYTE Mask) { return Operand&Mask; }
	static inline BYTE Get(BYTE Operand, BITS8 const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	static inline UINT Get(UINT Operand, UINT Mask) { return Operand&Mask; }
	static inline UINT Get(UINT Operand, BITS const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	static inline UINT Get(UINT64 Operand, UINT Mask) { return ((UINT)Operand)&Mask; }
	static inline UINT64 Get(UINT64 Operand, UINT64 Mask) { return Operand&Mask; }
	static inline VOID Set(UINT& Operand, UINT Mask) { Operand|=Mask; }
	static inline VOID Set(UINT& Operand, BITS32 const& Bits, UINT Value)
		{
		assert((Value&Bits.Mask)==Value);
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=(Value<<Bits.Shift);
		}
	static inline VOID Set(UINT64& Operand, UINT64 Mask) { Operand|=Mask; }
	static inline VOID Set(UINT& Operand, UINT Mask, UINT Value)
		{
		UINT value=Operand;
		value&=~Mask;
		value|=Value;
		Operand=value;
		}
	static inline VOID Set(UINT64& Operand, UINT64 Mask, UINT64 Value)
		{
		UINT64 value=Operand;
		value&=~Mask;
		value|=Value;
		Operand=value;
		}
	static inline VOID Switch(UINT& Operand, UINT Mask) { Operand^=Mask; }
};
