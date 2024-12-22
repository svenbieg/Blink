//=========
// Flags.h
//=========

#pragma once


//=======
// Using
//=======

#include "Collections/index.hpp"
#include "Variable.h"


//=======
// Flags
//=======

class Flags: public Variable
{
public:
	// Con-/Destructors
	Flags(Handle<String> Name);

	// Access
	BOOL Get(Handle<String> Flag);
	Handle<String> ToString(LanguageCode Language)override;
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	VOID Clear(BOOL Notify=true);
	VOID Clear(Handle<String> Flag, BOOL Notify=true);
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	VOID Set(Handle<String> Flag, BOOL Notify=true);

private:
	// Common
	Collections::index<Handle<String>> m_Flags;
};
