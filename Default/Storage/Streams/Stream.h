//==========
// Stream.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


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


//========
// Stream
//========

class Stream
{
public:
	// Using
	using StreamFormat=Storage::Streams::StreamFormat;

	// Settings
	static const StreamFormat DefaultStreamFormat=StreamFormat::UTF8;

	// Common
	StreamFormat GetStreamFormat()const { return m_StreamFormat; }
	virtual VOID SetStreamFormat(StreamFormat Format) { m_StreamFormat=Format; }

protected:
	// Con-/Destructors
	Stream(StreamFormat Format=DefaultStreamFormat): m_StreamFormat(Format) {}

	// Common
	StreamFormat m_StreamFormat;
};

}}