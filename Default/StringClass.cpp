//=================
// StringClass.cpp
//=================

#include "pch.h"


//========
// Access
//========

Handle<String> String::ToString()
{
return this;
}


//===========
// Operators
//===========

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL cs, BOOL repeat)
{
UINT len=StringHelper::Replace(nullptr, 0, m_Buffer, find, replace);
if(!len)
	return nullptr;
Handle<String> str=new String(len+1, nullptr);
LPTSTR buf=const_cast<LPTSTR>(str->Begin());
StringHelper::Replace(buf, len+1, m_Buffer, find, replace, cs, repeat);
return str;
}
