//===============
// BoolClass.cpp
//===============

#include "pch.h"


//=======
// Using
//=======

#include "BoolClass.h"


//==================
// Con-/Destructors
//==================

Bool::Bool(BOOL value):
Bool(nullptr, value)
{}

Bool::Bool(Handle<String> name, BOOL value):
TypedVariable(name, value)
{}


//==============
// Modification
//==============

BOOL Bool::FromString(Handle<String> value, BOOL notify)
{
BOOL b=false;
if(FromString(value, &b))
	{
	Set(b, notify);
	return true;
	}
return false;
}

BOOL Bool::FromString(Handle<String> value, BOOL* ptr)
{
if(!value)
	return false;
if(StringHelper::Compare(value->Begin(), "0", false)==0)
	{
	*ptr=false;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "1", false)==0)
	{
	*ptr=true;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "false", false)==0)
	{
	*ptr=false;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "true", false)==0)
	{
	*ptr=true;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "no", false)==0)
	{
	*ptr=false;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "yes", false)==0)
	{
	*ptr=true;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "nein", false)==0)
	{
	*ptr=false;
	return true;
	}
if(StringHelper::Compare(value->Begin(), "ja", false)==0)
	{
	*ptr=true;
	return true;
	}
return false;
}
