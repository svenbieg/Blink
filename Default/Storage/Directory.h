//=============
// Directory.h
//=============

#pragma once


//=======
// Using
//=======

#include "Storage/File.h"


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
	virtual Handle<DirectoryIterator> Begin()=0;
	virtual Handle<File> CreateFile(Handle<String> Path, FileCreateMode Create, FileAccessMode Access, FileShareMode Share)=0;
	virtual Handle<Object> Get(Handle<String> Path)=0;
	virtual Handle<String> GetName()=0;
	virtual Handle<Directory> GetParent()const=0;

protected:
	// Con-/Destructors
	Directory() {}
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