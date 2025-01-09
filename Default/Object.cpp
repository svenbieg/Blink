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

Handle<String> Object::ToString(LanguageCode lng)
{
return nullptr;
}


//================
// Common Private
//================

VOID Object::Release()
{
if(--m_RefCount==0)
	delete this;
}
