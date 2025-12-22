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

class Stream: public Object
{
public:
	// Using
	using StreamFormat=Storage::Streams::StreamFormat;

	// Common
	StreamFormat GetStreamFormat()const { return m_StreamFormat; }
	VOID SetStreamFormat(StreamFormat Format) { m_StreamFormat=Format; }

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

class RandomAccessStream: public InputStream, public OutputStream
{
protected:
	// Con-/Destructors
	RandomAccessStream(StreamFormat Format=DefaultStreamFormat): Stream(Format) {}
};


//==========
// Seekable
//==========

class Seekable: public InputStream, public OutputStream
{
public:
	// Common
	virtual FILE_SIZE GetSize()=0;
	virtual BOOL Seek(FILE_SIZE Position)=0;
};

}}