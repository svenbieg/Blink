//============
// Object.cpp
//============

// Reference-counted objects on the heap.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#include "Object.h"


//=======
// Using
//=======

#include "StringClass.h"


//================
// Common Private
//================

UINT Object::Release()noexcept
{
UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
if(ref_count==0)
	delete this;
return ref_count;
}
