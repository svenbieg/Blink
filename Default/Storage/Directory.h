//=============
// Directory.h
//=============

#pragma once


//=======
// Using
//=======

#include "File.h"


//===========
// Namespace
//===========

namespace Storage {


//======================
// Forward-Declarations
//======================

class DirectoryIterator;


//===========
// Directory
//===========

class Directory: public Object
{
public:
	// Common
	virtual Handle<File> CreateFile(Handle<String> Path, FileCreateMode CreateMode, FileAccessMode AccessMode, FileShareMode ShareMode)=0;
	virtual Handle<DirectoryIterator> First()=0;
	virtual Handle<Object> Get(Handle<String> Path)=0;
	Handle<String> GetName()const;
	Handle<String> GetPath()const;

protected:
	// Con-/Destructors
	Directory(Handle<String> Path);

	// Common
	Handle<String> hPath;
};


//==========
// Iterator
//==========

class DirectoryIterator: public Object
{
public:
	// Common
	virtual Handle<Object> GetCurrent()const=0;
	virtual BOOL HasCurrent()const=0;
	virtual BOOL MoveNext()=0;

protected:
	// Con-/Destructors
	DirectoryIterator() {}
};

}