//================
// FloatClass.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "FloatClass.h"


//========
// Access
//========

FLOAT Float::Get()
{
FLOAT value=m_Value;
Reading(this, value);
return value;
}

SIZE_T Float::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(FLOAT);
FLOAT value=Get();
return Stream->Write(&value, sizeof(FLOAT));
}


//==============
// Modification
//==============

BOOL Float::FromString(Handle<String> value, BOOL notify)
{
if(!value)
	return false;
FLOAT f;
if(value->Scan("%f", &f)!=1)
	return false;
return Set(f, notify);
}

SIZE_T Float::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(FLOAT);
FLOAT value;
SIZE_T size=stream->Read(&value, sizeof(FLOAT));
if(size==sizeof(FLOAT))
	Set(value, notify);
return size;
}

BOOL Float::Set(FLOAT value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}
