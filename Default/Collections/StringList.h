//==============
// StringList.h
//==============

#pragma once


//=======
// Using
//=======

#include "List.h"


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
	// Using
	using _base_t=List<Handle<String>>;

	// Con-/Destructors
	using _base_t::_base_t;
	StringList(Handle<String> String, CHAR Separator=';');
};

}