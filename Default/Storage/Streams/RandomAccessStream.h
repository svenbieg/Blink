//==========
// Stream.h
//==========

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "Object.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//======================
// Random-Access-Stream
//======================

class RandomAccessStream: public Object, public InputStream, public OutputStream
{
public:
	// Common
	StreamFormat GetStreamFormat()const override { return m_StreamFormat; }
	VOID SetStreamFormat(StreamFormat Format) { m_StreamFormat=Format; }

protected:
	// Con-/Destructors
	RandomAccessStream(StreamFormat Format=StreamFormat::UTF8): m_StreamFormat(Format) {}

	// Common
	StreamFormat m_StreamFormat;
};

}}