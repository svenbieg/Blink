//=========
// Dwarf.h
//=========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

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
	Dwarf(SIZE_T Position):
		m_Buffer((BYTE*)Position)
		{}
	Dwarf(Dwarf const& Dwarf):
		m_Buffer(Dwarf.m_Buffer)
		{}

	// Common
	inline LPCSTR Begin()const noexcept
		{
		return (LPCSTR)m_Buffer;
		}
	static UINT GetEncodedSize(BYTE Encoding);
	inline SIZE_T GetPosition()const noexcept
		{
		return (SIZE_T)m_Buffer;
		}
	inline UINT64 Read()
		{
		return Read((BYTE const*&)m_Buffer);
		}
	static UINT64 Read(BYTE const*& Dwarf);
	template <std::integral _int_t> static SIZE_T Read(InputStream* Stream, _int_t* Value)
		{
		if(!Stream)
			throw InvalidArgumentException();
		using _uint_t=std::make_unsigned<_int_t>::type;
		SIZE_T size=0;
		_uint_t value=0;
		UINT shift=0;
		BYTE byte=0;
		do
			{
			if(size==sizeof(_uint_t))
				throw BufferOverrunException();
			size+=Stream->Read(&byte, 1);
			value|=((UINT)byte&0x7F)<<shift;
			shift+=7;
			}
		while(byte&0x80);
		if(byte&0x40)
			value|=~0UL<<shift;
		if(Value)
			*Value=(_int_t)value;
		return size;
		}
	template <std::unsigned_integral _uint_t> static SIZE_T Read(InputStream* Stream, _uint_t* Value)
		{
		if(!Stream)
			throw InvalidArgumentException();
		SIZE_T size=0;
		_uint_t value=0;
		UINT shift=0;
		BYTE byte=0;
		do
			{
			if(size==sizeof(_uint_t))
				throw BufferOverrunException();
			size+=Stream->Read(&byte, 1);
			value|=((_uint_t)byte&0x7F)<<shift;
			shift+=7;
			}
		while(byte&0x80);
		if(Value)
			*Value=value;
		return size;
		}
	inline BYTE ReadByte()noexcept
		{
		return *m_Buffer++;
		}
	static inline BYTE ReadByte(BYTE const*& Dwarf)noexcept
		{
		return *Dwarf++;
		}
	inline UINT64 ReadEncoded(BYTE Encoding, SIZE_T DataRelative=0)
		{
		return ReadEncoded((BYTE const*&)m_Buffer, Encoding, DataRelative);
		}
	static UINT64 ReadEncoded(BYTE const*& Dwarf, BYTE Encoding, SIZE_T DataRelative=0);
	inline INT64 ReadSigned()
		{
		return ReadSigned((BYTE const*&)m_Buffer);
		}
	static INT64 ReadSigned(BYTE const*& Dwarf);
	inline UINT64 ReadUnsigned()
		{
		return ReadUnsigned((BYTE const*&)m_Buffer);
		}
	static UINT64 ReadUnsigned(BYTE const*& Dwarf);
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
	template <typename _value_t> static inline _value_t ReadValue(InputStream* Stream)
		{
		assert(Stream);
		_value_t value;
		Stream->Read(&value, sizeof(_value_t));
		return value;
		}
	VOID SetPosition(SIZE_T Position)noexcept
		{
		m_Buffer=(BYTE*)Position;
		}
	template <std::integral _int_t> static SIZE_T Write(OutputStream* Stream, _int_t Value)
		{
		BOOL neg=false;
		if(Value<0)
			{
			Value=-Value;
			neg=true;
			}
		using _uint_t=std::make_unsigned<_int_t>::type;
		_uint_t value=(_uint_t)Value;
		SIZE_T size=0;
		do
			{
			BYTE byte=(BYTE)value&0x7F;
			value>>=7;
			if(value)
				{
				byte|=0x80;
				}
			else if(neg)
				{
				if(byte&0x40)
					{
					value=0x80;
					byte|=0x80;
					}
				else
					{
					byte|=0x40;
					}
				}
			if(Stream)
				{
				size+=Stream->Write(&byte, 1);
				}
			else
				{
				size++;
				}
			}
		while(value);
		return size;
		}
	template <std::unsigned_integral _uint_t> static SIZE_T Write(OutputStream* Stream, _uint_t Value)
		{
		SIZE_T size=0;
		do
			{
			BYTE byte=(BYTE)Value&0x7F;
			Value>>=7;
			if(Value)
				byte|=0x80;
			if(Stream)
				{
				size+=Stream->Write(&byte, 1);
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