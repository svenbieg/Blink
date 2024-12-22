//==================
// IntegerClasses.h
//==================

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//=======
// Int32
//=======

class Int32: public TypedVariable<INT>
{
public:
	// Con-/Destructors
	Int32(INT Value=0): TypedVariable(nullptr, Value) {}
	Int32(Handle<String> Name, INT Value=0): TypedVariable(Name, Value) {}

	// Access
	Handle<String> ToString(LanguageCode Language)override { return new String("%i", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%i", Notify); }
};


//==============
// Int32-Handle
//==============

template <>
class Handle<Int32>: public ::Details::VariableHandle<Int32, INT>
{
public:
	// Using
	using _base_t=::Details::VariableHandle<Int32, INT>;
	using _base_t::_base_t;
};


//=======
// Int64
//=======

class Int64: public TypedVariable<INT64>
{
public:
	// Con-/Destructors
	Int64(INT64 Value=0): TypedVariable(nullptr, Value) {}
	Int64(Handle<String> Name, INT64 Value=0): TypedVariable(Name, Value) {}

	// Access
	Handle<String> ToString()override { return new String("%i", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%i", Notify); }
};


//==============
// Int64-Handle
//==============

template <>
class Handle<Int64>: public ::Details::VariableHandle<Int64, INT64>
{
public:
	// Using
	using _base_t=::Details::VariableHandle<Int64, INT64>;
	using _base_t::_base_t;
};
