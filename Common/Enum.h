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
	// Friends
	friend EnumIterator;

	// Using
	using Sentence=Culture::Sentence;

	// Con-/Destructors
	static Handle<Enum> Create(Handle<String> Name);

	// Access
	Handle<EnumIterator> Begin();
	inline Handle<Sentence> Get()const { return m_Value; }
	inline Handle<String> GetName()const override { return m_Name; }
	Handle<String> ToString(LanguageCode Language)override;
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	VOID Add(Handle<Sentence> Value);
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(Handle<Sentence> Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Enum(Handle<String> Name): m_Name(Name) {}

	// Common
	Handle<String> m_Name;
	Handle<Sentence> m_Value;
	Collections::index<Handle<Sentence>> m_Values;
};


//=============
// Enum-Handle
//=============

template <>
class Handle<Enum>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Using
	using Sentence=Culture::Sentence;
	using STRING=Resources::Strings::STRING;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(Enum* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator Enum*()const { return m_Object; }
	inline Enum* operator->()const { return m_Object; }
	inline operator Handle<Sentence>()const { return m_Object? m_Object->Get(): nullptr; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	inline BOOL operator==(Enum* Object)const { return m_Object==Object; }
	inline BOOL operator==(Handle const& Compare)const { return m_Object==Compare.m_Object; }
	inline BOOL operator==(STRING const* Value)const
		{
		Handle<Sentence> value=m_Object? m_Object->Get(): nullptr;
		if(!value)
			{
			if(Value)
				return false;
			return true;
			}
		return value->Compare(Value)==0;
		}
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(Enum* Object)const { return !operator==(Object); }
	inline BOOL operator!=(Handle const& Compare)const { return !operator==(Compare.m_Object); }
	inline BOOL operator!=(STRING const* Value)const { return !operator==(Value); }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(Enum* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	inline Handle& operator=(STRING const* Value)
		{
		assert(m_Object);
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	Enum* m_Object;
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
	EnumIterator(Handle<Enum> Enum): m_Enum(Enum), m_It(&Enum->m_Values) {}

	// Common
	Handle<Enum> m_Enum;
	typename Collections::index<Handle<Sentence>>::iterator m_It;
};
