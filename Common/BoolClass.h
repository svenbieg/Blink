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
	static Handle<Bool> Create(BOOL Value=false);
	static Handle<Bool> Create(Handle<String> Name, BOOL Value=false);

	// Access
	Handle<String> ToString(LanguageCode Language)override { return String::Create("%i", (INT)Get()); }

	// Modification
	BOOL FromString(Handle<String> String, BOOL Notify=true)override;
	static BOOL FromString(Handle<String> String, BOOL* Value);

private:
	// Con-/Destructors
	Bool(Handle<String> Name, BOOL Value);
};


//=============
// Bool-Handle
//=============

template <>
class Handle<Bool>: public VariableHandle<Bool, BOOL>
{
public:
	// Using
	using _base_t=VariableHandle<Bool, BOOL>;
	using _base_t::_base_t;

	// Modification
	inline Handle& operator=(BOOL Value) { Set(Value); return *this; }
};


//==================
// Con-/Destructors
//==================

inline Handle<Bool> Bool::Create(BOOL Value)
{
return new Bool(nullptr, Value);
}

inline Handle<Bool> Bool::Create(Handle<String> Name, BOOL Value)
{
return new Bool(Name, Value);
}
