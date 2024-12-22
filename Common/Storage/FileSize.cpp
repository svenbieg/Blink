//==============
// FileSize.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "FileSize.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

FileSize::FileSize(Handle<String> name, FILE_SIZE value):
TypedVariable(name, value)
{}


//========
// Common
//========

Handle<String> FileSize::ToString()
{
auto size=Get();
return ToString(size);
}

Handle<String> FileSize::ToString(FILE_SIZE size)
{
if(size>1024*1024*1024)
	return new String("%.2f GB", ((FLOAT)(size/1024/1024))/1024.f);
if(size>1024*1024)
	return new String("%.2f MB", ((FLOAT)(size/1024))/1024.f);
if(size>1024)
	return new String("%.2f KB", ((FLOAT)size)/1024.f);
return new String("%u Bytes", (UINT)size);
}

}