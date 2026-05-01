//============
// Unsigned.h
//============

#pragma once


//=======
// Using
//=======

#include "Storage/Encoding/Dwarf.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Encoding {


//==========
// Unsigned
//==========

class Unsigned
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Con-/Destructors
	inline Unsigned()noexcept: m_Value(0) {}
	inline Unsigned(UINT64 Value)noexcept: m_Value(Value) {}
	inline Unsigned(Unsigned const& Value)noexcept: m_Value(Value.m_Value) {}
	inline Unsigned(Unsigned&& Move)noexcept: m_Value(Move.m_Value) {}

	// Common
	inline operator UINT64()const noexcept { return m_Value; }
	inline SIZE_T ReadFromStream(InputStream* Stream) { return Dwarf::ReadUnsigned(Stream, &m_Value); }
	inline SIZE_T WriteToStream(OutputStream* Stream) { return Dwarf::WriteUnsigned(Stream, m_Value); }

private:
	// Common
	UINT64 m_Value;
};

}}