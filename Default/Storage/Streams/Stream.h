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


//==============
// Input-Stream
//==============

class InputStream: public virtual Stream
{
public:
	// Common
	virtual SIZE_T Available()=0;
	virtual SIZE_T Read(VOID* Buffer, SIZE_T Size)=0;
};


//===============
// Output-Stream
//===============

class OutputStream: public virtual Stream
{
public:
	// Common
	virtual VOID Flush()=0;
	virtual SIZE_T Write(VOID const* Buffer, SIZE_T Size)=0;
};


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


//==========
// Seekable
//==========

class Seekable: public RandomAccessStream
{
public:
	// Common
	virtual SIZE_T GetSize()=0;
	virtual BOOL Seek(SIZE_T Position)=0;

protected:
	// Con-/Destructors
	Seekable(StreamFormat Format=StreamFormat::UTF8): RandomAccessStream(Format) {}
};

}}