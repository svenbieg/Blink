//========
// Enum.h
//========

#pragma once


//=======
// Using
//=======

#include "Collections/index.hpp"
#include "Culture/Sentence.h"
#include "Variable.h"


//======================
// Forward-Declarations
//======================

class EnumIterator;


//======
// Enum
//======

class Enum: public Variable
{
public:
	// Using
	using Sentence=Culture::Sentence;

	// Friends
	friend EnumIterator;

	// Con-/Destructors
	static Handle<Enum> Create(Handle<String> Name);

	// Access
	Handle<EnumIterator> First();
	Handle<Sentence> Get();
	Handle<String> ToString(LanguageCode Language)override;
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	VOID Add(Handle<Sentence> Value);
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(Handle<Sentence> Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Enum(Handle<String> Name);

	// Common
	Handle<Sentence> m_Value;
	Collections::index<Handle<Sentence>> m_Values;
};


//=============
// Enum-Handle
//=============

template <>
class Handle<Enum>: public HandleBase<Enum>
{
public:
	// Using
	using _base_t=HandleBase<Enum>;
	using _base_t::_base_t;
	using Sentence=Culture::Sentence;
	using STRING=Resources::Strings::STRING;

	// Access
	inline operator Handle<Sentence>()const { return Get(); }
	Handle<Sentence> Get()const
		{
		if(!m_Object)
			return nullptr;
		return m_Object->Get();
		}

	// Comparison
	inline bool operator==(STRING const* Value)const
		{
		Handle<Sentence> value=Get();
		if(!value)
			{
			if(Value)
				return false;
			return true;
			}
		return value->Compare(Value)==0;
		}
	inline BOOL operator!=(STRING const* Value)const { return !operator==(Value); }

	// Assignment
	inline Handle& operator=(STRING const* Value)
		{
		if(m_Object)
			m_Object->Set(Value);
		return *this;
		}
};


//==================
// Con-/Destructors
//==================

inline Handle<Enum> Enum::Create(Handle<String> Name)
{
return new Enum(Name);
}


//==========
// Iterator
//==========

class EnumIterator: public Object
{
public:
	// Using
	using Sentence=Culture::Sentence;

	// Friend
	friend Enum;

	// Con-/Destructors
	~EnumIterator();

	// Access
	Handle<Sentence> GetCurrent()const { return m_It.get_current(); }
	UINT GetPosition()const { return m_It.get_position(); }
	BOOL HasCurrent()const { return m_It.has_current(); }

	// Navigation
	BOOL Begin() { return m_It.begin(); }
	BOOL End() { return m_It.rbegin(); }
	BOOL MoveNext() { return m_It.move_next(); }
	BOOL MovePrevious() { return m_It.move_previous(); }
	BOOL SetPosition(UINT Position) { return m_It.set_position(Position); }

private:
	// Con-/Destructors
	EnumIterator(Handle<Enum> Enum);

	// Common
	Handle<Enum> m_Enum;
	typename Collections::index<Handle<Sentence>>::iterator m_It;
};
