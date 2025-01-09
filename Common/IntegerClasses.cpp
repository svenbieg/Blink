//====================
// IntegerClasses.cpp
//====================

#include "pch.h"


//=======
// Using
//=======

#include "IntegerClasses.h"


//================
// Int32 - Access
//================

INT Int32::Get()
{
INT value=m_Value;
Reading(this, value);
return value;
}

SIZE_T Int32::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(INT);
INT value=Get();
return Stream->Write(&value, sizeof(INT));
}


//======================
// Int32 - Modification
//======================

BOOL Int32::FromString(Handle<String> str, BOOL notify)
{
INT value;
if(FromString(str, &value))
	{
	Set(value, notify);
	return true;
	}
return false;
}

BOOL Int32::FromString(Handle<String> str, INT* value_ptr)
{
if(!str)
	return false;
INT value;
if(str->Scan("%i", &value)!=1)
	return false;
if(value_ptr)
	*value_ptr=value;
return true;
}

SIZE_T Int32::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(INT);
INT value;
SIZE_T size=stream->Read(&value, sizeof(INT));
if(size==sizeof(INT))
	Set(value, notify);
return size;
}

BOOL Int32::Set(INT value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}


//================
// Int64 - Access
//================

INT64 Int64::Get()
{
INT64 value=m_Value;
Reading(this, value);
return value;
}

SIZE_T Int64::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(INT64);
INT64 value=Get();
return Stream->Write(&value, sizeof(INT64));
}


//======================
// Int64 - Modification
//======================

BOOL Int64::FromString(Handle<String> str, BOOL notify)
{
INT64 value;
if(FromString(str, &value))
	{
	Set(value, notify);
	return true;
	}
return false;
}

BOOL Int64::FromString(Handle<String> str, INT64* value_ptr)
{
if(!str)
	return false;
INT64 value;
if(str->Scan("%i", &value)!=1)
	return false;
if(value_ptr)
	*value_ptr=value;
return true;
}

SIZE_T Int64::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(INT64);
INT64 value;
SIZE_T size=stream->Read(&value, sizeof(INT64));
if(size==sizeof(INT64))
	Set(value, notify);
return size;
}

BOOL Int64::Set(INT64 value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}
