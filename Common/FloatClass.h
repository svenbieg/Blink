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

class Float: public Variable
{
public:
	// Con-/Destructors
	static Handle<Float> Create(FLOAT Value=0.f);
	static Handle<Float> Create(Handle<String> Name, FLOAT Value=0.f);

	// Access
	FLOAT Get();
	static inline FLOAT Get(Float* Value) { return Value? Value->Get(): 0.f; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Float, FLOAT&> Reading;
	inline Handle<String> ToString(LanguageCode Language)override { return String::Create("%.2f", Get()); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(FLOAT Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Float(Handle<String> Name, FLOAT Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	FLOAT m_Value;
};


//==============
// Float-Handle
//==============

template <>
class Handle<Float>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(Float* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return Float::Get(m_Object)!=0.f; }
	inline operator Float*()const { return m_Object; }
	inline Float* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return Float::Get(m_Object)==0.f; }
	inline BOOL operator==(Float* Value)const { return Float::Get(m_Object)==Float::Get(Value); }
	inline BOOL operator==(FLOAT Value)const { return Float::Get(m_Object)==Value; }
	inline BOOL operator!=(nullptr_t)const { return Float::Get(m_Object)!=0.f; }
	inline BOOL operator!=(Float* Value)const { return Float::Get(m_Object)!=Float::Get(Value); }
	inline BOOL operator!=(FLOAT Value)const { return Float::Get(m_Object)!=Value; }
	inline BOOL operator>(nullptr_t)const { return Float::Get(m_Object)>0.f; }
	inline BOOL operator>(Float* Value)const { return Float::Get(m_Object)>Float::Get(Value); }
	inline BOOL operator>(FLOAT Value)const { return Float::Get(m_Object)>Value; }
	inline BOOL operator>=(nullptr_t)const { return Float::Get(m_Object)>=0.f; }
	inline BOOL operator>=(Float* Value)const { return Float::Get(m_Object)>=Float::Get(Value); }
	inline BOOL operator>=(FLOAT Value)const { return Float::Get(m_Object)>=Value; }
	inline BOOL operator<(nullptr_t)const { return Float::Get(m_Object)<0.f; }
	inline BOOL operator<(Float* Value)const { return Float::Get(m_Object)<Float::Get(Value); }
	inline BOOL operator<(FLOAT Value)const { return Float::Get(m_Object)<Value; }
	inline BOOL operator<=(nullptr_t)const { return Float::Get(m_Object)<=0.f; }
	inline BOOL operator<=(Float* Value)const { return Float::Get(m_Object)<=Float::Get(Value); }
	inline BOOL operator<=(FLOAT Value)const { return Float::Get(m_Object)<=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(Float* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	Handle& operator=(FLOAT Value)
		{
		if(!m_Object)
			{
			auto value=Float::Create(Value);
			return operator=(value);
			}
		m_Object->Set(Value);
		return *this;
		}

private:
	// Common
	Float* m_Object;
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
