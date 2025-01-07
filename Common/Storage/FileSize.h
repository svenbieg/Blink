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
	static inline Handle<FileSize> Create(FILE_SIZE Value=0)
		{
		return new FileSize(nullptr, Value);
		}
	static inline Handle<FileSize> Create(Handle<String> Name, FILE_SIZE Value=0)
		{
		return new FileSize(Name, Value);
		}

	// Common
	Handle<String> ToString()override;
	static Handle<String> ToString(FILE_SIZE Size);

private:
	// Con-/Destructors
	FileSize(Handle<String> Name, FILE_SIZE Value): TypedVariable(Name, Value) {}
};

}