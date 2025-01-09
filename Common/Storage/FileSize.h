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

class FileSize: public Variable
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

	// Access
	FILE_SIZE Get();
	static inline FILE_SIZE Get(FileSize* Value) { return Value? Value->Get(): 0; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, FILE_SIZE&> Reading;
	inline Handle<String> ToString(LanguageCode Language=LanguageCode::None)override { return ToString(Get()); }
	static Handle<String> ToString(FILE_SIZE Size);
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(FILE_SIZE Value, BOOL Notify=true);

private:
	// Con-/Destructors
	FileSize(Handle<String> Name, FILE_SIZE Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	FILE_SIZE m_Value;
};

}