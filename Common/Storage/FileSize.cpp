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


//========
// Access
//========

FILE_SIZE FileSize::Get()
{
FILE_SIZE value=m_Value;
Reading(this, value);
return value;
}

Handle<String> FileSize::ToString(FILE_SIZE size)
{
if(size>1024*1024*1024)
	return String::Create("%.2f GB", ((FLOAT)(size/1024/1024))/1024.f);
if(size>1024*1024)
	return String::Create("%.2f MB", ((FLOAT)(size/1024))/1024.f);
if(size>1024)
	return String::Create("%.2f KB", ((FLOAT)size)/1024.f);
return String::Create("%u Bytes", (UINT)size);
}

SIZE_T FileSize::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(FILE_SIZE);
FILE_SIZE value=Get();
return Stream->Write(&value, sizeof(FILE_SIZE));
}


//==============
// Modification
//==============

SIZE_T FileSize::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(FILE_SIZE);
FILE_SIZE value;
SIZE_T size=stream->Read(&value, sizeof(FILE_SIZE));
if(size==sizeof(FILE_SIZE))
	Set(value, notify);
return size;
}

BOOL FileSize::Set(FILE_SIZE value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}

}