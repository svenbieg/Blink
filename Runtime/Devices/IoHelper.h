//============
// IoHelper.h
//============

#pragma once


//=======
// Using
//=======

#include "BitHelper.h"


//===========
// Namespace
//===========

namespace Devices {


//===========
// Registers
//===========

typedef volatile const BYTE RO8;
typedef volatile const WORD RO16;
typedef volatile const UINT RO32;
typedef volatile const UINT64 RO64;

typedef volatile BYTE RW08;
typedef volatile WORD RW16;
typedef volatile UINT RW32;
typedef volatile UINT64 RW64;


//===========
// IO-Helper
//===========

class IoHelper
{
public:
	// Common
	static inline VOID Clear(RW08& Register, BYTE Mask) { Register&=~Mask; }
	static inline VOID Clear(RW16& Register, WORD Mask) { Register&=~Mask; }
	static inline VOID Clear(RW32& Register, UINT Mask) { Register&=~Mask; }
	static inline VOID Clear(RW64& Register, UINT64 Mask) { Register&=~Mask; }
	static inline BYTE Read(RO8& Register) { return Register; }
	static inline UINT Read(RO8& Register, BYTE Mask) { return Register&Mask; }
	static inline UINT Read(RO8& Register, BITS8 const& Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline WORD Read(RO16& Register) { return Register; }
	static inline UINT Read(RO16& Register, WORD Mask) { return Register&Mask; }
	static inline UINT Read(RO16& Register, BITS16 const& Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline UINT Read(RO32& Register) { return Register; }
	static inline UINT Read(RO32& Register, UINT Mask) { return Register&Mask; }
	static inline UINT Read(RO32& Register, BITS const& Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline UINT64 Read(RO64& Register) { return Register; }
	static inline UINT64 Read(RO64& Register, UINT64 Mask) { return Register&Mask; }
	static inline UINT64 Read(RO64& Register, BITS64 const& Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline VOID Set(RW08& Register, BYTE Mask) { Register|=Mask; }
	static inline VOID Set(RW08& Register, BYTE Mask, BYTE Value)
		{
		BYTE tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW08& Register, BITS8 const& Bits, BYTE Value)
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW16& Register, WORD Mask) { Register|=Mask; }
	static inline VOID Set(RW16& Register, WORD Mask, WORD Value)
		{
		WORD tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW16& Register, BITS16 const& Bits, WORD Value)
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW32& Register, UINT Mask) { Register|=Mask; }
	static inline VOID Set(RW32& Register, UINT Mask, UINT Value)
		{
		UINT tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW32& Register, BITS const& Bits, UINT Value)
		{
		UINT tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW64& Register, UINT64 Mask) { Register|=Mask; }
	static inline VOID Set(RW64& Register, UINT64 Mask, UINT64 Value)
		{
		UINT64 tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW64& Register, BITS64 const& Bits, WORD Value)
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Write(RW08& Register, BYTE Value) { Register=Value; }
	static inline VOID Write(RW16& Register, WORD Value) { Register=Value; }
	static inline VOID Write(RW32& Register, UINT Value) { Register=Value; }
	static inline VOID Write(RW64& Register, UINT64 Value) { Register=Value; }
};

}