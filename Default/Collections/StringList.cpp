//================
// StringList.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "StringList.h"


//===========
// Namespace
//===========

namespace Collections {


//==========================
// Con-/Destructors Private
//==========================

StringList::StringList(Handle<String> str, CHAR sep)
{
if(!str)
	return;
auto str_ptr=str->Begin();
UINT pos=0;
while(StringHelper::FindChar(str_ptr, sep, &pos))
	{
	auto value=String::Create(pos, str_ptr);
	m_List.append(value);
	str_ptr=&str_ptr[pos+1];
	pos=0;
	}
if(str_ptr[pos])
	{
	auto value=String::Create(str_ptr);
	m_List.append(value);
	}
}

}