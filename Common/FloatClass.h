//==============
// FloatClass.h
//==============

#pragma once


//=======
// Using
//=======

#include "Variable.h"


//=======
// Float
//=======

class Float: public TypedVariable<FLOAT>
{
public:
	// Con-/Destructors
	Float(FLOAT Value=0.f): TypedVariable(nullptr, Value) {}
	Float(Handle<String> Name, FLOAT Value=0.f): TypedVariable(Name, Value) {}

	// Access
	Handle<String> ToString(LanguageCode Language)override { return new String("%.2f", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%f", Notify); }
};


//==============
// Float-Handle
//==============

template <>
class Handle<Float>: public ::Details::VariableHandle<Float, FLOAT>
{
public:
	// Using
	using _base_t=::Details::VariableHandle<Float, FLOAT>;
	using _base_t::_base_t;
};
