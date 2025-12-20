//==============
// StringList.h
//==============

#pragma once


//=======
// Using
//=======

#include "Collections/List.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Collections {


//=============
// String-List
//=============

class StringList: public List<Handle<String>>
{
public:
	// Con-/Destructors
	static inline Handle<StringList> Create() { return new StringList(); }
	static inline Handle<StringList> Create(Handle<String> String, CHAR Separator=';') { return new StringList(String, Separator); }

private:
	// Con-/Destructors
	StringList() {}
	StringList(Handle<String> String, CHAR Separator);
};

}