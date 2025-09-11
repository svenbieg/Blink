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

UINT Object::Release()
{
UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
if(ref_count==0)
	delete this;
return ref_count;
}
