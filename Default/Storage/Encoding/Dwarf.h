//=========
// Dwarf.h
//=========

// DWARF was orginally developed by Bell Labs,
// licensed under the GNU Free Documentation Standard

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"


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
	inline LPCSTR Begin()const { return (LPCSTR)m_Buffer; }
	static UINT GetEncodedSize(BYTE Encoding);
	inline SIZE_T GetPosition()const { return (SIZE_T)m_Buffer; }
	inline UINT64 Read() { return Read((BYTE const*&)m_Buffer); }
	static UINT64 Read(BYTE const*& Dwarf);
	inline BYTE ReadByte() { return *m_Buffer++; }
	static inline BYTE ReadByte(BYTE const*& Dwarf) { return *Dwarf++; }
	inline UINT64 ReadEncoded(BYTE Encoding, SIZE_T Relative=0) { return ReadEncoded((BYTE const*&)m_Buffer, Encoding, Relative); }
	static UINT64 ReadEncoded(BYTE const*& Dwarf, BYTE Encoding, UINT64 Relative=0);
	inline INT64 ReadSigned() { return ReadSigned((BYTE const*&)m_Buffer); }
	static INT64 ReadSigned(BYTE const*& Dwarf);
	inline UINT64 ReadUnsigned() { return ReadUnsigned((BYTE const*&)m_Buffer); }
	static UINT64 ReadUnsigned(BYTE const*& Dwarf);
	template <typename _value_t> static UINT ReadUnsigned(InputStream* Dwarf, _value_t* Value)
		{
		if(!Dwarf)
			throw InvalidArgumentException();
		_value_t value=0;
		UINT size=0;
		UINT shift=0;
		BYTE byte=0;
		do
			{
			UINT read=(UINT)Dwarf->Read(&byte, 1);
			if(read!=1)
				break;
			if(size==sizeof(_value_t))
				throw BufferOverrunException();
			size+=read;
			value|=((UINT)byte&0x7F)<<shift;
			shift+=7;
			}
		while(byte&0x80);
		if(Value)
			*Value=value;
		return size;
		}
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
	template <typename _value_t> static inline _value_t ReadValue(InputStream* Dwarf)
		{
		_value_t value;
		SIZE_T read=Dwarf->Read(&value, sizeof(_value_t));
		if(read!=sizeof(_value_t))
			throw DeviceNotReadyException();
		return value;
		}
	VOID SetPosition(SIZE_T Position) { m_Buffer=(BYTE*)Position; }
	static UINT WriteSigned(OutputStream* Dwarf, INT64 Value);
	template <typename _value_t> static UINT WriteUnsigned(OutputStream* Dwarf, _value_t Value)
		{
		UINT size=0;
		do
			{
			if(size==sizeof(_value_t))
				throw BufferOverrunException();
			BYTE byte=(BYTE)Value&0x7F;
			Value>>=7;
			if(Value)
				byte|=0x80;
			if(Dwarf)
				{
				size+=(UINT)Dwarf->Write(&byte, 1);
				}
			else
				{
				size++;
				}
			}
		while(Value);
		return size;
		}

private:
	// Common
	BYTE* m_Buffer;
};

}}