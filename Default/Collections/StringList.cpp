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


//==================
// Con-/Destructors
//==================

StringList::StringList(Handle<String> str, CHAR sep)
{
if(!str)
	return;
auto str_ptr=str->Begin();
UINT pos=0;
while(StringFindChar(str_ptr, sep, &pos))
	{
	Handle<String> value=new String(pos, str_ptr);
	m_List.append(value);
	str_ptr=&str_ptr[pos+1];
	pos=0;
	}
if(str_ptr[pos])
	{
	Handle<String> value=new String(str_ptr);
	m_List.append(value);
	}
}

}