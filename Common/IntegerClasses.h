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
	static Handle<Int32> Create(INT Value=0);
	static Handle<Int32> Create(Handle<String> Name, INT Value=0);

	// Access
	Handle<String> ToString(LanguageCode Language)override { return String::Create("%i", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%i", Notify); }

private:
	// Con-/Destructors
	Int32(Handle<String> Name, INT Value): TypedVariable(Name, Value) {}

};


//==============
// Int32-Handle
//==============

template <>
class Handle<Int32>: public VariableHandle<Int32, INT>
{
public:
	// Using
	using _base_t=VariableHandle<Int32, INT>;
	using _base_t::_base_t;

	// Modification
	inline Handle& operator=(INT Value) { Set(Value); return *this; }
};


//========================
// Int32 Con-/Destructors
//========================

inline Handle<Int32> Int32::Create(INT Value)
{
return new Int32(nullptr, Value);
}

inline Handle<Int32> Int32::Create(Handle<String> Name, INT Value)
{
return new Int32(Name, Value);
}


//=======
// Int64
//=======

class Int64: public TypedVariable<INT64>
{
public:
	// Con-/Destructors
	static Handle<Int64> Create(INT64 Value=0);
	static Handle<Int64> Create(Handle<String> Name, INT64 Value=0);

	// Access
	Handle<String> ToString()override { return String::Create("%i", Get()); }

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return TypedVariable::FromString(Value, "%i", Notify); }

private:
	// Con-/Destructors
	Int64(Handle<String> Name, INT64 Value): TypedVariable(Name, Value) {}
};


//==============
// Int64-Handle
//==============

template <>
class Handle<Int64>: public VariableHandle<Int64, INT64>
{
public:
	// Using
	using _base_t=VariableHandle<Int64, INT64>;
	using _base_t::_base_t;

	// Modification
	inline Handle& operator=(INT64 Value) { Set(Value); return *this; }
};


//========================
// Int64 Con-/Destructors
//========================

inline Handle<Int64> Int64::Create(INT64 Value)
{
return new Int64(nullptr, Value);
}

inline Handle<Int64> Int64::Create(Handle<String> Name, INT64 Value)
{
return new Int64(Name, Value);
}
