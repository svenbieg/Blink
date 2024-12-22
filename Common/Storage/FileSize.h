//============
// FileSize.h
//============

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//===========
// Namespace
//===========

namespace Storage {


//===========
// File-Size
//===========

class FileSize: public TypedVariable<FILE_SIZE>
{
public:
	// Con-/Destructors
	FileSize(Handle<String> Name, FILE_SIZE Value=0);

	// Common
	Handle<String> ToString()override;
	static Handle<String> ToString(FILE_SIZE Size);
};

}