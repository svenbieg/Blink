//=============
// BoolClass.h
//=============

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//======
// Bool
//======

class Bool: public TypedVariable<BOOL>
{
public:
	// Using
	using LanguageCode=Culture::LanguageCode;

	// Con-/Destructors
	Bool(BOOL Value=false);
	Bool(Handle<String> Name, BOOL Value=false);

	// Access
	Handle<String> ToString(LanguageCode Language)override { return new String("%i", (INT)Get()); }

	// Modification
	BOOL FromString(Handle<String> String, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> String, BOOL* Value);
};


//=============
// Bool-Handle
//=============

template <>
class Handle<Bool>: public Details::VariableHandle<Bool, BOOL>
{
public:
	// Using
	using _base_t=Details::VariableHandle<Bool, BOOL>;
	using _base_t::_base_t;
};
