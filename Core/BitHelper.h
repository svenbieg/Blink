//=============
// BitHelper.h
//=============

#pragma once


//==========
// Register
//==========

typedef volatile BYTE REG8;
typedef volatile WORD REG16;
typedef volatile UINT REG;


//===========
// Bit-Field
//===========

typedef struct
{
UINT Mask;
UINT Shift;
}BIT_FIELD;


//======
// Bits
//======

class Bits
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
	static inline UINT Get(volatile UINT& Operand, BIT_FIELD const& Bits)
		{
		return (Operand>>Bits.Shift)&Bits.Mask;
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
	static inline VOID Set(volatile WORD& Operand, BIT_FIELD const& Bits, WORD Value)
		{
		WORD value=Operand;
		value&=~(Bits.Mask<<Bits.Shift);
		value|=(Value<<Bits.Shift);
		Operand=value;
		}
	static inline VOID Set(volatile UINT& Operand, BIT_FIELD const& Bits, UINT Value)
		{
		UINT value=Operand;
		value&=~(Bits.Mask<<Bits.Shift);
		value|=(Value<<Bits.Shift);
		Operand=value;
		}
	static inline VOID Set(volatile UINT64& Operand, BIT_FIELD const& Bits, UINT64 Value)
		{
		UINT64 value=Operand;
		value&=~((UINT64)Bits.Mask<<Bits.Shift);
		value|=(Value<<Bits.Shift);
		Operand=value;
		}
	static inline VOID Switch(volatile UINT& Operand, UINT Mask)
		{
		Operand^=Mask;
		}
	static inline VOID Write(volatile BYTE& Operand, BYTE Value)
		{
		Operand=Value;
		}
	static inline VOID Write(volatile WORD& Operand, WORD Value)
		{
		Operand=Value;
		}
	static inline VOID Write(volatile UINT& Operand, UINT Value)
		{
		Operand=Value;
		}
};
