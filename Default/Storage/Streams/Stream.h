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
constexpr StreamFormat StandardStreamFormat=StreamFormat::Unicode;
#else
constexpr StreamFormat StandardStreamFormat=StreamFormat::Ansi;
#endif


//========
// Stream
//========

class Stream: public virtual Object
{
public:
	// Common
	StreamFormat GetFormat() { return uFormat; }
	VOID SetFormat(StreamFormat Format) { uFormat=Format; }

protected:
	// Con-/Destructors
	Stream(): uFormat(StandardStreamFormat) {}

	// Common
	StreamFormat uFormat;
};

}}
