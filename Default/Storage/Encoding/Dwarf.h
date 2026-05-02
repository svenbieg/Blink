//=========
// Dwarf.h
//=========

// DWARF was orginally developed by Bell Labs and is
// licensed under the GNU Free Documentation Standard.

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {


//==========
// Encoding
//==========

typedef enum
{
DW_POINTER_ABS=0,
DW_UNSIGNED=1,
DW_WORD=2,
DW_DWORD=3,
DW_QWORD=4,
DW_SIGNED=9,
DW_SHORT=10,
DW_INT=11,
DW_LONG=12,
DW_POINTER_REL=0x10,
DW_TEXT_REL=0x20,
DW_DATA_REL=0x30,
DW_FUNC_REL=0x40,
DW_ALIGNED=0x50,
DW_INDIRECT=0x80,
DW_OMIT=0xFF
}DW_ENCODING;


//=======
// Dwarf
//=======

class Dwarf
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Con-/Destructors
	Dwarf(SIZE_T Position): m_Buffer((BYTE*)Position) {}
	Dwarf(Dwarf const& Dwarf): m_Buffer(Dwarf.m_Buffer) {}

	// Common
	inline LPCSTR Begin()const noexcept { return (LPCSTR)m_Buffer; }
	static UINT GetEncodedSize(BYTE Encoding);
	inline SIZE_T GetPosition()const noexcept { return (SIZE_T)m_Buffer; }
	inline UINT64 Read() { return Read((BYTE const*&)m_Buffer); }
	static UINT64 Read(BYTE const*& Dwarf);
	inline BYTE ReadByte()noexcept { return *m_Buffer++; }
	static inline BYTE ReadByte(BYTE const*& Dwarf)noexcept { return *Dwarf++; }
	inline UINT64 ReadEncoded(BYTE Encoding, SIZE_T DataRelative=0) { return ReadEncoded((BYTE const*&)m_Buffer, Encoding, DataRelative); }
	static UINT64 ReadEncoded(BYTE const*& Dwarf, BYTE Encoding, UINT64 DataRelative=0);
	inline INT64 ReadSigned()noexcept { return ReadSigned((BYTE const*&)m_Buffer); }
	static INT64 ReadSigned(BYTE const*& Dwarf)noexcept;
	static inline UINT ReadSigned(InputStream* Dwarf, INT* Value)
		{
		INT64 value=0;
		UINT size=ReadSigned(Dwarf, &value);
		if(Value)
			*Value=TypeHelper::Integral<INT, INT64>(value);
		return size;
		}
	static UINT ReadSigned(InputStream* Dwarf, INT64* Value);
	inline UINT64 ReadUnsigned()noexcept { return ReadUnsigned((BYTE const*&)m_Buffer); }
	static UINT64 ReadUnsigned(BYTE const*& Dwarf)noexcept;
	static inline UINT ReadUnsigned(InputStream* Dwarf, UINT* Value)
		{
		UINT64 value=0;
		UINT size=ReadUnsigned(Dwarf, &value);
		if(Value)
			*Value=TypeHelper::Integral<UINT, UINT64>(value);
		return size;
		}
	static UINT ReadUnsigned(InputStream* Dwarf, UINT64* Value);
	template <typename _value_t> inline _value_t ReadValue()noexcept
		{
		_value_t value;
		MemoryHelper::Copy(&value, m_Buffer, sizeof(_value_t));
		m_Buffer+=sizeof(_value_t);
		return value;
		}
	template <typename _value_t> static inline _value_t ReadValue(BYTE const*& Dwarf)noexcept
		{
		_value_t value;
		MemoryHelper::Copy(&value, Dwarf, sizeof(_value_t));
		Dwarf+=sizeof(_value_t);
		return value;
		}
	template <typename _value_t> static inline _value_t ReadValue(InputStream* Dwarf)
		{
		assert(Dwarf);
		_value_t value;
		Dwarf->Read(&value, sizeof(_value_t));
		return value;
		}
	VOID SetPosition(SIZE_T Position)noexcept { m_Buffer=(BYTE*)Position; }
	static UINT WriteSigned(OutputStream* Dwarf, INT64 Value);
	static UINT WriteUnsigned(OutputStream* Dwarf, UINT64 Value);

private:
	// Common
	BYTE* m_Buffer;
};

}}