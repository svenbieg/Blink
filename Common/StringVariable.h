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
	// Con-/Destructors
	StringVariable(Handle<String> Name, Handle<String> Value=nullptr);

	// Access
	Handle<String> Get();
	Event<Variable, Handle<String>&> Reading;
	Handle<String> ToString(LanguageCode Language)override { return Get(); }
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Modification
	BOOL FromString(Handle<String> Value, BOOL Notify=true)override;
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify)override;
	BOOL Set(Handle<String> Value, BOOL Notify=true);

private:
	// Common
	Handle<String> m_Value;
};


//========================
// Handle String-Variable
//========================

template <>
class Handle<StringVariable>: public ::Details::HandleBase<StringVariable>
{
public:
	// Using
	using _base_t=::Details::HandleBase<StringVariable>;
	using _base_t::_base_t;

	// Con-/Destructors
	Handle(Handle<String> Id, Handle<String> Value) { Create(new StringVariable(Id, Value)); }

	// Access
	operator Handle<String>()const { return m_Object? m_Object->Get(): nullptr; }

	// Comparison
	BOOL operator==(nullptr_t)const override
		{
		if(!m_Object)
			return true;
		return m_Object->Get()==nullptr;
		}
	BOOL operator==(LPCSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()==Value;
		}
	BOOL operator==(LPCWSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()==Value;
		}
	BOOL operator==(Handle<String> const& Handle)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()==Handle;
		}
	inline BOOL operator!=(LPCSTR Value)const { return !operator==(Value); }
	inline BOOL operator!=(LPCWSTR Value)const { return !operator==(Value); }
	inline BOOL operator!=(Handle<String> const& Handle)const { return !operator==(Handle); }
	BOOL operator>(decltype(nullptr))const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>nullptr;
		}
	BOOL operator>(LPCSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>Value;
		}
	BOOL operator>(LPCWSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>Value;
		}
	BOOL operator>(Handle<String> const& Handle)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>Handle;
		}
	BOOL operator>=(decltype(nullptr))const { return true; }
	BOOL operator>=(LPCSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>=Value;
		}
	BOOL operator>=(LPCWSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>=Value;
		}
	BOOL operator>=(Handle<String> const& Handle)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()>=Handle;
		}
	BOOL operator<(decltype(nullptr))const { return false; }
	BOOL operator<(LPCSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<Value;
		}
	BOOL operator<(LPCWSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<Value;
		}
	BOOL operator<(Handle<String> const& Handle)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<Handle;
		}
	BOOL operator<=(decltype(nullptr))const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<=nullptr;
		}
	BOOL operator<=(LPCSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<=Value;
		}
	BOOL operator<=(LPCWSTR Value)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<=Value;
		}
	BOOL operator<=(Handle<String> const& Handle)const
		{
		if(!m_Object)
			return false;
		return m_Object->Get()<=Handle;
		}

	// Assignment
	Handle& operator=(LPCSTR Value)
		{
		if(m_Object)
			{
			m_Object->Set(Value);
			}
		else
			{
			Create(new StringVariable(Value));
			}
		return *this;
		}
	Handle& operator=(LPCWSTR Value)
		{
		if(m_Object)
			{
			m_Object->Set(Value);
			}
		else
			{
			Create(new StringVariable(Value));
			}
		return *this;
		}
	Handle& operator=(String* String)
		{
		if(m_Object)
			{
			m_Object->Set(String);
			}
		else
			{
			Create(new StringVariable(String));
			}
		return *this;
		}
};
