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
	static inline VOID Clear(BYTE& Operand, BYTE Mask) { Operand&=~Mask; }
	static inline VOID Clear(BYTE& Operand, BITS8 const& Bits) { Operand&=~(Bits.Mask<<Bits.Shift); }
	static inline VOID Clear(WORD& Operand, WORD Mask) { Operand&=~Mask; }
	static inline VOID Clear(WORD& Operand, BITS16 const& Bits) { Operand&=~(Bits.Mask<<Bits.Shift); }
	static inline VOID Clear(UINT& Operand, UINT Mask) { Operand&=~Mask; }
	static inline VOID Clear(UINT& Operand, BITS32 const& Bits) { Operand&=~(Bits.Mask<<Bits.Shift); }
	static inline VOID Clear(UINT64& Operand, UINT64 Mask) { Operand&=~Mask; }
	static inline VOID Clear(UINT64& Operand, BITS64 const& Bits) { Operand&=~(Bits.Mask<<Bits.Shift); }
	static inline BYTE Get(BYTE Operand, BYTE Mask) { return Operand&Mask; }
	static inline BYTE Get(BYTE Operand, BITS8 const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	static inline WORD Get(WORD Operand, WORD Mask) { return Operand&Mask; }
	static inline WORD Get(WORD Operand, BITS16 const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	static inline UINT Get(UINT Operand, UINT Mask) { return Operand&Mask; }
	static inline UINT Get(UINT Operand, BITS32 const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	static inline UINT64 Get(UINT64 Operand, UINT64 Mask) { return Operand&Mask; }
	static inline UINT64 Get(UINT64 Operand, BITS64 const& Bits) { return (Operand>>Bits.Shift)&Bits.Mask; }
	template <class _op_t> static inline UINT Get(_op_t Operand, UINT Mask) { return (UINT)Operand&Mask; }
	static inline VOID Set(BYTE& Operand, BYTE Mask) { Operand|=Mask; }
	static inline VOID Set(BYTE& Operand, BYTE Mask, BYTE Value)
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static inline VOID Set(BYTE& Operand, BITS8 const& Bits, BYTE Value)
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static inline VOID Set(WORD& Operand, WORD Mask) { Operand|=Mask; }
	static inline VOID Set(WORD& Operand, WORD Mask, WORD Value)
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static inline VOID Set(WORD& Operand, BITS16 const& Bits, WORD Value)
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static inline VOID Set(UINT& Operand, UINT Mask) { Operand|=Mask; }
	static inline VOID Set(UINT& Operand, UINT Mask, UINT Value)
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static inline VOID Set(UINT& Operand, BITS32 const& Bits, UINT Value)
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static inline VOID Set(UINT64& Operand, UINT64 Mask) { Operand|=Mask; }
	static inline VOID Set(UINT64& Operand, UINT64 Mask, UINT64 Value)
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static inline VOID Set(UINT64& Operand, BITS64 const& Bits, UINT64 Value)
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static inline VOID Switch(BYTE& Operand, BYTE Mask) { Operand^=Mask; }
	static inline VOID Switch(WORD& Operand, WORD Mask) { Operand^=Mask; }
	static inline VOID Switch(UINT& Operand, UINT Mask) { Operand^=Mask; }
	static inline VOID Switch(UINT64& Operand, UINT64 Mask) { Operand^=Mask; }
};
