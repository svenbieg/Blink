//====================
// StringVariable.cpp
//====================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "StringVariable.h"

using namespace Storage::Streams;


//========
// Access
//========

Handle<String> StringVariable::Get()
{
SharedLock lock(m_Mutex);
Handle<String> value=m_Value;
lock.Unlock();
Reading(this, value);
return value;
}

SIZE_T StringVariable::WriteToStream(OutputStream* stream)
{
SharedLock lock(m_Mutex);
Handle<String> value=m_Value;
lock.Unlock();
Reading(this, value);
SIZE_T size=0;
StreamWriter writer(stream);
size+=writer.Print(value);
size+=writer.PrintChar('\0');
return size;
}


//==============
// Modification
//==============

BOOL StringVariable::FromString(Handle<String> value, BOOL notify)
{
return Set(value, notify);
}

SIZE_T StringVariable::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return 0;
SIZE_T size=0;
StreamReader reader(stream);
auto str=reader.ReadString();
Set(str, notify);
return size;
}

BOOL StringVariable::Set(Handle<String> value, BOOL notify)
{
ScopedLock lock(m_Mutex);
if(m_Value==value)
	return true;
m_Value=value;
lock.Unlock();
if(notify)
	Changed(this);
return true;
}


//==========================
// Con-/Destructors Private
//==========================

StringVariable::StringVariable(Handle<String> name, Handle<String> value):
Variable(name),
m_Value(value)
{}
