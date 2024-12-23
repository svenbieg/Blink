//=========
// Dwarf.h
//=========

// DWARF was orginally developed by Bell Labs,
// licensed under the GNU Free Documentation Standard

#pragma once


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
	// Con-/Destructors
	Dwarf(SIZE_T Position): m_Buffer((BYTE*)Position) {}
	Dwarf(Dwarf const& Dwarf): m_Buffer(Dwarf.m_Buffer) {}

	// Common
	inline LPCSTR Begin()const { return (LPCSTR)m_Buffer; }
	static UINT GetEncodedSize(BYTE Encoding);
	inline SIZE_T GetPosition()const { return (SIZE_T)m_Buffer; }
	inline SIZE_T Read() { return Read((BYTE const*&)m_Buffer); }
	static SIZE_T Read(BYTE const*& Dwarf);
	inline BYTE ReadByte() { return *m_Buffer++; }
	static inline BYTE ReadByte(BYTE const*& Dwarf) { return *Dwarf++; }
	inline SIZE_T ReadEncoded(BYTE Encoding, SIZE_T Relative=0) { return ReadEncoded((BYTE const*&)m_Buffer, Encoding, Relative); }
	static SIZE_T ReadEncoded(BYTE const*& Dwarf, BYTE Encoding, SIZE_T Relative=0);
	inline SIZE_T ReadSigned() { return ReadSigned((BYTE const*&)m_Buffer); }
	static SIZE_T ReadSigned(BYTE const*& Dwarf);
	inline SIZE_T ReadUnsigned() { return ReadUnsigned((BYTE const*&)m_Buffer); }
	static SIZE_T ReadUnsigned(BYTE const*& Dwarf);
	template <typename _value_t> inline _value_t ReadValue()
		{
		_value_t value=*(_value_t const*)m_Buffer;
		m_Buffer+=sizeof(_value_t);
		return value;
		}
	template <typename _value_t> static inline _value_t ReadValue(BYTE const*& Dwarf)
		{
		_value_t value=*(_value_t const*)Dwarf;
		Dwarf+=sizeof(_value_t);
		return value;
		}
	VOID SetPosition(SIZE_T Position) { m_Buffer=(BYTE*)Position; }

private:
	// Common
	BYTE* m_Buffer;
};

}}