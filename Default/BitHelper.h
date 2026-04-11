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
	static constexpr VOID Clear(BYTE& Operand, BYTE Mask)noexcept { Operand&=~Mask; }
	static constexpr VOID Clear(BYTE& Operand, BITS8 const& Bits)noexcept { Operand&=~(Bits.Mask<<Bits.Shift); }
	static constexpr VOID Clear(WORD& Operand, WORD Mask)noexcept { Operand&=~Mask; }
	static constexpr VOID Clear(WORD& Operand, BITS16 const& Bits)noexcept { Operand&=~(Bits.Mask<<Bits.Shift); }
	static constexpr VOID Clear(UINT& Operand, UINT Mask)noexcept { Operand&=~Mask; }
	static constexpr VOID Clear(UINT& Operand, BITS32 const& Bits)noexcept { Operand&=~(Bits.Mask<<Bits.Shift); }
	static constexpr VOID Clear(UINT64& Operand, UINT64 Mask)noexcept { Operand&=~Mask; }
	static constexpr VOID Clear(UINT64& Operand, BITS64 const& Bits)noexcept { Operand&=~(Bits.Mask<<Bits.Shift); }
	static constexpr BYTE Get(BYTE Operand, BYTE Mask)noexcept { return Operand&Mask; }
	static constexpr BYTE Get(BYTE Operand, BITS8 const& Bits)noexcept { return (Operand>>Bits.Shift)&Bits.Mask; }
	static constexpr WORD Get(WORD Operand, WORD Mask)noexcept { return Operand&Mask; }
	static constexpr WORD Get(WORD Operand, BITS16 const& Bits)noexcept { return (Operand>>Bits.Shift)&Bits.Mask; }
	static constexpr UINT Get(UINT Operand, UINT Mask)noexcept { return Operand&Mask; }
	static constexpr UINT Get(UINT Operand, BITS32 const& Bits)noexcept { return (Operand>>Bits.Shift)&Bits.Mask; }
	static constexpr UINT64 Get(UINT64 Operand, UINT64 Mask)noexcept { return Operand&Mask; }
	static constexpr UINT64 Get(UINT64 Operand, BITS64 const& Bits)noexcept { return (Operand>>Bits.Shift)&Bits.Mask; }
	template <class _op_t> static constexpr UINT Get(_op_t Operand, UINT Mask)noexcept { return (UINT)Operand&Mask; }
	static constexpr VOID Set(BYTE& Operand, BYTE Mask)noexcept { Operand|=Mask; }
	static constexpr VOID Set(BYTE& Operand, BYTE Mask, BYTE Value)noexcept
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static constexpr VOID Set(BYTE& Operand, BITS8 const& Bits, BYTE Value)noexcept
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static constexpr VOID Set(WORD& Operand, WORD Mask)noexcept { Operand|=Mask; }
	static constexpr VOID Set(WORD& Operand, WORD Mask, WORD Value)noexcept
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static constexpr VOID Set(WORD& Operand, BITS16 const& Bits, WORD Value)noexcept
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static constexpr VOID Set(UINT& Operand, UINT Mask)noexcept { Operand|=Mask; }
	static constexpr VOID Set(UINT& Operand, UINT Mask, UINT Value)noexcept
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static constexpr VOID Set(UINT& Operand, BITS32 const& Bits, UINT Value)noexcept
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static constexpr VOID Set(UINT64& Operand, UINT64 Mask)noexcept { Operand|=Mask; }
	static constexpr VOID Set(UINT64& Operand, UINT64 Mask, UINT64 Value)noexcept
		{
		Operand&=~Mask;
		Operand|=(Value&Mask);
		}
	static constexpr VOID Set(UINT64& Operand, BITS64 const& Bits, UINT64 Value)noexcept
		{
		Operand&=~(Bits.Mask<<Bits.Shift);
		Operand|=((Value&Bits.Mask)<<Bits.Shift);
		}
	static constexpr VOID Switch(BYTE& Operand, BYTE Mask)noexcept { Operand^=Mask; }
	static constexpr VOID Switch(WORD& Operand, WORD Mask)noexcept { Operand^=Mask; }
	static constexpr VOID Switch(UINT& Operand, UINT Mask)noexcept { Operand^=Mask; }
	static constexpr VOID Switch(UINT64& Operand, UINT64 Mask)noexcept { Operand^=Mask; }
};
