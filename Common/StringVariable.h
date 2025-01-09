//==================
// StringVariable.h
//==================

#pragma once


//=======
// Using
//=======

#include "StringClass.h"
#include "Variable.h"


//=================
// String-Variable
//=================

class StringVariable: public Variable
{
public:
	// Friends
	friend class Handle<StringVariable>;

	// Con-/Destructors
	static Handle<StringVariable> Create(Handle<String> Name, Handle<String> Value=nullptr);

	// Access
	Handle<String> Get();
	static inline Handle<String> Get(StringVariable* Value) { return Value? Value->Get(): nullptr; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, Handle<String>&> Reading;
	Handle<String> ToString(LanguageCode Language)override { return Get(); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	inline BOOL FromString(Handle<String> Value, BOOL Notify=true)override { return Set(Value, Notify); }
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify)override;
	BOOL Set(Handle<String> Value, BOOL Notify=true);

private:
	// Con-/Destructors
	StringVariable(Handle<String> Name, Handle<String> Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	Handle<String> m_Value;
};


//========
// Handle
//========

template <>
class Handle<StringVariable>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): m_Object(nullptr) {}
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(StringVariable* Object): m_Object(Object)
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	Handle(Handle&& Move)noexcept: m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	~Handle()
		{
		if(m_Object)
			{
			m_Object->Release();
			m_Object=nullptr;
			}
		}

	// Access
	inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator StringVariable*()const { return m_Object; }
	inline StringVariable* operator->()const { return m_Object; }
	operator Handle<String>()const { return m_Object? m_Object->Get(): nullptr; }

	// Comparison
	BOOL operator==(nullptr_t)const { return StringVariable::Get(m_Object)==nullptr; }
	BOOL operator==(StringVariable* Value)const { return StringVariable::Get(m_Object)==StringVariable::Get(Value); }
	BOOL operator==(LPCSTR Value)const { return StringVariable::Get(m_Object)==Value; }
	BOOL operator==(LPCWSTR Value)const { return StringVariable::Get(m_Object)==Value; }
	BOOL operator!=(nullptr_t)const { return StringVariable::Get(m_Object)!=nullptr; }
	BOOL operator!=(StringVariable* Value)const { return StringVariable::Get(m_Object)!=StringVariable::Get(Value); }
	BOOL operator!=(LPCSTR Value)const { return StringVariable::Get(m_Object)!=Value; }
	BOOL operator!=(LPCWSTR Value)const { return StringVariable::Get(m_Object)!=Value; }

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		this->~Handle();
		return *this;
		}
	Handle& operator=(StringVariable* Object)
		{
		if(m_Object==Object)
			return *this;
		if(m_Object)
			m_Object->Release();
		m_Object=Object;
		if(m_Object)
			m_Object->m_RefCount++;
		return *this;
		}
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	inline Handle& operator=(LPCSTR Value)
		{
		if(m_Object)
			{
			m_Object->Set(Value);
			return *this;
			}
		auto value=StringVariable::Create(nullptr, Value);
		return operator=(value);
		}
	inline Handle& operator=(LPCWSTR Value)
		{
		if(m_Object)
			{
			m_Object->Set(Value);
			return *this;
			}
		auto value=StringVariable::Create(nullptr, Value);
		return operator=(value);
		}
	inline Handle& operator=(String* Value)
		{
		if(m_Object)
			{
			m_Object->Set(Value);
			return *this;
			}
		auto value=StringVariable::Create(nullptr, Value);
		return operator=(value);
		}

private:
	// Common
	StringVariable* m_Object;
};


//==================
// Con-/Destructors
//==================

inline Handle<StringVariable> StringVariable::Create(Handle<String> Name, Handle<String> Value)
{
return new StringVariable(Name, Value);
}
