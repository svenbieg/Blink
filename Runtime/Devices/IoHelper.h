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
	static inline VOID Clear(RW32& Register, UINT Mask) { Register&=~Mask; }
	static inline BYTE Read(RO8& Register) { return Register; }
	static inline WORD Read(RO16& Register) { return Register; }
	static inline UINT Read(RO32& Register) { return Register; }
	static inline UINT Read(RO32& Register, UINT Mask) { return Register&Mask; }
	static inline UINT Read(RO32& Register, BITS Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline UINT64 Read(RO64& Register) { return Register; }
	static inline UINT64 Read(RO64& Register, UINT64 Mask) { return Register&Mask; }
	static inline UINT64 Read(RO64& Register, BITS64 Bits) { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline VOID Set(RW32& Register, UINT Mask) { Register|=Mask; }
	static inline VOID Write(RW08& Register, BYTE value) { Register=value; }
	static inline VOID Write(RW16& Register, WORD value) { Register=value; }
	static inline VOID Write(RW16& Register, BITS16 Bits, WORD value)
		{
		WORD u=Register;
		u&=~(Bits.Mask<<Bits.Shift);
		u|=(value<<Bits.Shift);
		Register=u;
		}
	static inline VOID Write(RW32& Register, UINT value) { Register=value; }
	static inline VOID Write(RW32& Register, UINT Mask, UINT value)
		{
		UINT u=Register;
		u&=~Mask;
		u|=value;
		Register=u;
		}
	static inline VOID Write(RW32& Register, BITS Bits, UINT value)
		{
		UINT u=Register;
		u&=~(Bits.Mask<<Bits.Shift);
		u|=(value<<Bits.Shift);
		Register=u;
		}
	static inline VOID Write(RW64& Register, UINT64 value) { Register=value; }
};

}