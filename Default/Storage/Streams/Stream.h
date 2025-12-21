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


//=========
// IStream
//=========

class IStream
{
public:
	// Common
	virtual StreamFormat GetStreamFormat()const=0;
	virtual VOID SetStreamFormat(StreamFormat Format)=0;
};


//========
// Stream
//========

class Stream: public Object, public virtual IStream
{
public:
	// Using
	using StreamFormat=Storage::Streams::StreamFormat;

	// Common
	StreamFormat GetStreamFormat()const override { return m_StreamFormat; }
	VOID SetStreamFormat(StreamFormat Format)override { m_StreamFormat=Format; }

protected:
	// Using
	#ifdef _UNICODE
	static constexpr StreamFormat DefaultStreamFormat=StreamFormat::Unicode;
	#else
	static constexpr StreamFormat DefaultStreamFormat=StreamFormat::Ansi;
	#endif

	// Con-/Destructors
	Stream(StreamFormat Format=DefaultStreamFormat): m_StreamFormat(Format) {}

	// Common
	StreamFormat m_StreamFormat;
};


//==============
// IInputStream
//==============

class IInputStream: public virtual IStream
{
public:
	// Common
	virtual SIZE_T Available()=0;
	virtual SIZE_T Read(VOID* Buffer, SIZE_T Size)=0;
};


//===============
// IOutputStream
//===============

class IOutputStream: public virtual IStream
{
public:
	// Common
	virtual VOID Flush()=0;
	virtual SIZE_T Write(VOID const* Buffer, SIZE_T Size)=0;
};


//======================
// Random-Access-Stream
//======================

class RandomAccessStream: public Stream, public IInputStream, public IOutputStream
{
protected:
	// Con-/Destructors
	RandomAccessStream(StreamFormat Format=DefaultStreamFormat): Stream(Format) {}

};


//===========
// ISeekable
//===========

class ISeekable: public IInputStream, public IOutputStream
{
public:
	// Common
	virtual FILE_SIZE GetSize()=0;
	virtual BOOL Seek(FILE_SIZE Position)=0;
};


//==========
// Seekable
//==========

class Seekable: public Stream, public ISeekable
{
protected:
	// Con-/Destructors
	Seekable(StreamFormat Format=DefaultStreamFormat): Stream(Format) {}

};

}}