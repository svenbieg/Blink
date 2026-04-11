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

typedef volatile const BYTE RO08;
typedef volatile const WORD RO16;
typedef volatile const UINT RO32;
typedef volatile const UINT64 RO64;

typedef volatile BYTE WO08;
typedef volatile WORD WO16;
typedef volatile UINT WO32;
typedef volatile UINT64 WO64;

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
	static inline VOID Clear(RW08& Register, BYTE Mask)noexcept { Register&=~Mask; }
	static inline VOID Clear(RW16& Register, WORD Mask)noexcept { Register&=~Mask; }
	static inline VOID Clear(RW32& Register, UINT Mask)noexcept { Register&=~Mask; }
	static inline VOID Clear(RW64& Register, UINT64 Mask)noexcept { Register&=~Mask; }
	static inline VOID Flip(RW32& Register, UINT Mask)noexcept { Register^=Mask; }
	static inline BYTE Read(RO08& Register)noexcept { return Register; }
	static inline UINT Read(RO08& Register, BYTE Mask)noexcept { return Register&Mask; }
	static inline UINT Read(RO08& Register, BITS8 const& Bits)noexcept { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline WORD Read(RO16& Register)noexcept { return Register; }
	static inline UINT Read(RO16& Register, WORD Mask)noexcept { return Register&Mask; }
	static inline UINT Read(RO16& Register, BITS16 const& Bits)noexcept { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline UINT Read(RO32& Register)noexcept { return Register; }
	static inline UINT Read(RO32& Register, UINT Mask)noexcept { return Register&Mask; }
	static inline UINT Read(RO32& Register, BITS const& Bits)noexcept { return (Register>>Bits.Shift)&Bits.Mask; }
	static inline UINT64 Read(RO64& Register)noexcept { return Register; }
	static inline UINT64 Read(RO64& Register, UINT64 Mask)noexcept { return Register&Mask; }
	static inline UINT64 Read(RO64& Register, BITS64 const& Bits)noexcept { return (Register>>Bits.Shift)&Bits.Mask; }
	static VOID Retry(RO32& Register, UINT Mask, UINT Value, UINT Retry=10);
	static inline VOID Set(RW08& Register, BYTE Mask)noexcept { Register|=Mask; }
	static inline VOID Set(RW08& Register, BYTE Mask, BYTE Value)noexcept
		{
		BYTE tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW08& Register, BITS8 const& Bits, BYTE Value)noexcept
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW16& Register, WORD Mask)noexcept { Register|=Mask; }
	static inline VOID Set(RW16& Register, WORD Mask, WORD Value)noexcept
		{
		WORD tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW16& Register, BITS16 const& Bits, WORD Value)noexcept
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW32& Register, UINT Mask)noexcept { Register|=Mask; }
	static inline VOID Set(RW32& Register, UINT Mask, UINT Value)noexcept
		{
		UINT tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW32& Register, BITS const& Bits, UINT Value)noexcept
		{
		UINT tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static inline VOID Set(RW64& Register, UINT64 Mask)noexcept { Register|=Mask; }
	static inline VOID Set(RW64& Register, UINT64 Mask, UINT64 Value)noexcept
		{
		UINT64 tmp=Register;
		tmp&=~Mask;
		tmp|=Value;
		Register=tmp;
		}
	static inline VOID Set(RW64& Register, BITS64 const& Bits, WORD Value)noexcept
		{
		WORD tmp=Register;
		tmp&=~(Bits.Mask<<Bits.Shift);
		tmp|=(Value<<Bits.Shift);
		Register=tmp;
		}
	static VOID Wait(RO32& Register, UINT Mask, UINT Value, UINT Timeout=100);
	static inline VOID Write(RW08& Register, BYTE Value)noexcept { Register=Value; }
	static inline VOID Write(RW16& Register, WORD Value)noexcept { Register=Value; }
	static inline VOID Write(RW32& Register, UINT Value)noexcept { Register=Value; }
	static inline VOID Write(RW64& Register, UINT64 Value)noexcept { Register=Value; }
};

}