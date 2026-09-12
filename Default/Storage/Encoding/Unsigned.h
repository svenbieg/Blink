//============
// Unsigned.h
//============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

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
	inline Unsigned()noexcept:
		m_Value(0)
		{}
	inline Unsigned(UINT Value)noexcept:
		m_Value(Value)
		{}
	inline Unsigned(Unsigned const& Value)noexcept:
		m_Value(Value.m_Value)
		{}

	// Common
	inline operator UINT()const noexcept
		{
		return m_Value;
		}
	inline SIZE_T ReadFromStream(InputStream* Stream)
		{
		return Dwarf::Read(Stream, &m_Value);
		}
	inline SIZE_T WriteToStream(OutputStream* Stream)
		{
		return Dwarf::Write(Stream, m_Value);
		}

private:
	// Common
	UINT m_Value;
};

}}