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

	// Common
	virtual StreamFormat GetStreamFormat()const=0;
};

}}