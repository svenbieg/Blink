//========
// File.h
//========

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Storage {


//=============
// Create-Mode
//=============

enum class FileCreateMode
{
OpenAlways,
OpenExisting,
CreateAlways,
CreateNew
};


//=============
// Access-Mode
//=============

enum class FileAccessMode
{
ReadOnly,
ReadWrite
};


//============
// Share-Mode
//============

enum class FileShareMode
{
Exclusive,
ShareRead,
ShareWrite
};


//======
// File
//======

class File: public Streams::Seekable
{
public:
	// Common
	virtual Handle<String> GetName()=0;
	virtual Handle<String> GetPath()=0;
	virtual BOOL SetSize(SIZE_T Size)=0;

protected:
	// Con-/Destructors
	File()=default;
};

}