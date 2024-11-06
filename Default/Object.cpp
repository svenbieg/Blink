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
if(--m_RefCount==0)
	delete this;
}

Handle<String> Object::ToString()
{
return nullptr;
}
