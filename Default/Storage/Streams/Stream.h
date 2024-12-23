//==========
// Stream.h
//==========

#pragma once


//=======
// Using
//=======

#include "Object.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// Stream-Format
//===============

enum class StreamFormat
{
Ansi,
Unicode,
UTF8
};


//=================
// Standard-Format
//=================

#ifdef UNICODE
constexpr StreamFormat DefaultStreamFormat=StreamFormat::Unicode;
#else
constexpr StreamFormat DefaultStreamFormat=StreamFormat::Ansi;
#endif


//========
// Stream
//========

class Stream: public virtual Object
{
public:
	// Common
	StreamFormat GetFormat() { return m_Format; }
	VOID SetFormat(StreamFormat Format) { m_Format=Format; }

protected:
	// Con-/Destructors
	Stream(): m_Format(DefaultStreamFormat) {}

	// Common
	StreamFormat m_Format;
};

}}
