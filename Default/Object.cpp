//============
// Object.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Object.h"
#include "StringClass.h"


//========
// Common
//========

VOID Object::Release()
{
if(--uRefCount==0)
	delete this;
}

Handle<String> Object::ToString()
{
return nullptr;
}
