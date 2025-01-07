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
	static Handle<Float> Create(FLOAT Value=0.f);
	static Handle<Float> Create(Handle<String> Name, FLOAT Value=0.f);

	// Access
	Handle<String> ToString(LanguageCode Language)override { return String::Create("%.2f", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%f", Notify); }

private:
	// Con-/Destructors
	Float(Handle<String> Name, FLOAT Value): TypedVariable(Name, Value) {}
};


//==============
// Float-Handle
//==============

template <>
class Handle<Float>: public VariableHandle<Float, FLOAT>
{
public:
	// Using
	using _base_t=VariableHandle<Float, FLOAT>;
	using _base_t::_base_t;

	// Modification
	inline Handle& operator=(FLOAT Value) { Set(Value); return *this; }
};


//==================
// Con-/Destructors
//==================

inline Handle<Float> Float::Create(FLOAT Value)
{
return new Float(nullptr, Value);
}

inline Handle<Float> Float::Create(Handle<String> Name, FLOAT Value)
{
return new Float(Name, Value);
}
