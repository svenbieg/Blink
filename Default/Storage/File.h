//========
// File.h
//========

#pragma once


//=======
// Using
//=======

#include "Seekable.h"


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
None,
ReadOnly,
ReadWrite
};


//============
// Share-Mode
//============

enum class FileShareMode
{
None,
Exclusive,
ShareRead,
ShareWrite
};


//======
// File
//======

class File: public Seekable
{
public:
	// Common
	virtual VOID Close()=0;
	virtual Status Create(FileCreateMode Create=FileCreateMode::OpenExisting, FileAccessMode Access=FileAccessMode::ReadOnly, FileShareMode Share=FileShareMode::ShareRead)=0;
	Handle<String> GetName()const;
	Handle<String> GetPath()const;
	virtual BOOL SetSize(FILE_SIZE Size)=0;

protected:
	// Con-/Destructors
	File(Handle<String> Path);

	// Common
	Handle<String> m_Path;
};

}