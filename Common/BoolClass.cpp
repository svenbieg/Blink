//===============
// BoolClass.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "Resources/Strings/Bool.h"
#include "BoolClass.h"

using namespace Resources::Strings;


//========
// Access
//========

BOOL Bool::Get()
{
BOOL value=m_Value;
Reading(this, value);
return value;
}

SIZE_T Bool::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(BOOL);
BOOL value=Get();
return Stream->Write(&value, sizeof(BOOL));
}


//==============
// Modification
//==============

BOOL Bool::FromString(Handle<String> str, BOOL notify)
{
BOOL value;
if(FromString(str, &value))
	{
	Set(value, notify);
	return true;
	}
return false;
}

BOOL Bool::FromString(Handle<String> str, BOOL* value_ptr)
{
if(!str)
	return false;
for(UINT u=0; u<TypeHelper::ArraySize(STR_TRUE); u++)
	{
	if(StringHelper::Compare(str, STR_TRUE[u].Value, false)==0)
		{
		if(value_ptr)
			*value_ptr=true;
		return true;
		}
	}
for(UINT u=0; u<TypeHelper::ArraySize(STR_FALSE); u++)
	{
	if(StringHelper::Compare(str, STR_FALSE[u].Value, false)==0)
		{
		if(value_ptr)
			*value_ptr=false;
		return true;
		}
	}
return false;
}

SIZE_T Bool::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(BOOL);
BOOL value;
SIZE_T size=stream->Read(&value, sizeof(BOOL));
if(size==sizeof(BOOL))
	Set(value, notify);
return size;
}

BOOL Bool::Set(BOOL value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}
