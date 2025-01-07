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
for(UINT u=0; u<TypeHelper::ArraySize(STR_TRUE); u++)
	{
	if(StringHelper::Compare(value, STR_TRUE[u].Value, false)==0)
		{
		*ptr=true;
		return true;
		}
	}
for(UINT u=0; u<TypeHelper::ArraySize(STR_FALSE); u++)
	{
	if(StringHelper::Compare(value, STR_FALSE[u].Value, false)==0)
		{
		*ptr=false;
		return true;
		}
	}
return false;
}


//==========================
// Con-/Destructors Private
//==========================

Bool::Bool(Handle<String> name, BOOL value):
TypedVariable(name, value)
{}
