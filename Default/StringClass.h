//===============
// StringClass.h
//===============

#pragma once


//=======
// Using
//=======

#include <new>
#include <utility>
#include "Handle.h"
#include "StringHelper.h"


//======================
// Forward-Declarations
//======================

class String;
template <> class Handle<String>;


//========
// String
//========

class String: public Object
{
public:
	// Con-/Destructors
	static Handle<String> Create(LPCSTR Value);
	static Handle<String> Create(LPCWSTR Value);
	static Handle<String> Create(UINT Length, nullptr_t);
	static Handle<String> Create(UINT Length, LPCSTR Value);
	static Handle<String> Create(UINT Length, LPCWSTR Value);
	static Handle<String> Create(LPCSTR Format, VariableArguments const& Arguments);
	template <class... _args_t> static inline Handle<String> Create(LPCSTR Format, _args_t... Arguments);

	// Access
	inline LPCTSTR Begin()const { return m_Buffer; }
	inline BOOL Contains(LPCSTR Value, BOOL CaseSensitive=true)
		{
		return StringHelper::Find(m_Buffer, Value, nullptr, CaseSensitive);
		}
	inline BOOL Contains(LPCWSTR Value, BOOL CaseSensitive=true)
		{
		return StringHelper::Find(m_Buffer, Value, nullptr, CaseSensitive);
		}
	UINT GetLength();
	inline BOOL HasValue()const { return m_Buffer[0]!=0; }
	inline BOOL IsEmpty()const { return m_Buffer[0]==0; }
	template <class... _args_t> UINT Scan(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return StringHelper::ScanArgs(m_Buffer, Format, vargs);
		}
	Handle<String> ToString()override;

	// Comparison
	inline BOOL operator==(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)==0; }
	inline BOOL operator!=(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)!=0; }
	inline BOOL operator>(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)>0; }
	inline BOOL operator>=(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)>=0; }
	inline BOOL operator<(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)<0; }
	inline BOOL operator<=(String const& String)const { return StringHelper::Compare(m_Buffer, String.m_Buffer)<=0; }
	INT Compare(String* String);

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);

private:
	// Settings
	static constexpr UINT64 INVALID_HASH=(1ULL<<63);

	// Con-/Destructors
	String(nullptr_t): m_Hash(INVALID_HASH), m_Length(0)
		{
		m_Buffer[0]=0;
		}
	String(UINT Size, LPCSTR Value): m_Hash(INVALID_HASH)
		{
		m_Length=StringHelper::Copy(m_Buffer, Size, Value);
		}
	String(UINT Size, LPCWSTR Value): m_Hash(INVALID_HASH)
		{
		m_Length=StringHelper::Copy(m_Buffer, Size, Value);
		}
	String(UINT Size, LPCSTR Format, VariableArguments const& Arguments): m_Hash(INVALID_HASH)
		{
		m_Length=StringHelper::PrintArgs(m_Buffer, Size, Format, Arguments);
		}

	// Common
	static UINT64 Hash(String* String);
	UINT64 m_Hash;
	UINT m_Length;

	// Buffer
	TCHAR m_Buffer[];
};


//===============
// String-Handle
//===============

template <>
class Handle<String>: public HandleBase<String>
{
public:
	// Using
	using _base_t=HandleBase<String>;

	// Con-/Destructors
	Handle(): _base_t(nullptr) {}
	Handle(nullptr_t): _base_t(nullptr) {}
	Handle(String* Object): _base_t(Object) {}
	Handle(Handle const& Copy): _base_t(Copy) {}
	Handle(Handle&& Move)noexcept: _base_t(Move.m_Object) { Move.m_Object=nullptr; }
	Handle(LPCSTR Value) { Create(Value); }
	Handle(LPCWSTR Value) { Create(Value); }
	template <class... _args_t> Handle(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		Create(Format, vargs);
		}

	// Access
	inline operator BOOL()const override { return m_Object&&m_Object->HasValue(); }

	// Comparison
	inline BOOL operator==(nullptr_t)const override { return !operator BOOL(); }
	inline BOOL operator==(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)==0;
		}
	inline BOOL operator==(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)==0;
		}
	inline BOOL operator==(String* Object)const override
		{
		if(!m_Object)
			return StringHelper::Compare((LPCTSTR)nullptr, Object? Object->Begin(): nullptr)==0;
		return m_Object->Compare(Object)==0;
		}
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(LPCSTR Value)const { return !operator==(Value); }
	inline BOOL operator!=(LPCWSTR Value)const { return !operator==(Value); }
	inline BOOL operator!=(String* Object)const { return !operator==(Object); }
	inline BOOL operator>(nullptr_t)const { return (m_Object&&m_Object->HasValue()); }
	inline BOOL operator>(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)>0;
		}
	inline BOOL operator>(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)>0;
		}
	inline BOOL operator>(String* Object)const
		{
		if(!m_Object)
			return false;
		return m_Object->Compare(Object)>0;
		}
	inline BOOL operator>=(nullptr_t)const { return true; }
	inline BOOL operator>=(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)>=0;
		}
	inline BOOL operator>=(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)>=0;
		}
	inline BOOL operator>=(String* Object)const
		{
		if(!m_Object)
			return Object? !Object->HasValue(): true;
		return m_Object->Compare(Object)>=0;
		}
	inline BOOL operator<(nullptr_t)const { return false; }
	inline BOOL operator<(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)<0;
		}
	inline BOOL operator<(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)<0;
		}
	inline BOOL operator<(String* Object)const
		{
		if(!m_Object)
			return Object? Object->HasValue(): false;
		return m_Object->Compare(Object)<0;
		}
	inline BOOL operator<=(nullptr_t)const { return !(m_Object&&m_Object->HasValue()); }
	inline BOOL operator<=(LPCSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)<=0;
		}
	inline BOOL operator<=(LPCWSTR Value)const
		{
		auto str=m_Object? m_Object->Begin(): nullptr;
		return StringHelper::Compare(str, Value)<=0;
		}
	inline BOOL operator<=(String* Object)const
		{
		if(!m_Object)
			return true;
		return m_Object->Compare(Object)<=0;
		}

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		Clear();
		return *this;
		}
	inline Handle& operator=(LPCSTR Value)
		{
		Set(Value);
		return *this;
		}
	inline Handle& operator=(LPCWSTR Value)
		{
		Set(Value);
		return *this;
		}
	inline Handle& operator=(Handle const& Copy)
		{
		_base_t::Set(Copy.m_Object);
		return *this;
		}
	VOID Set(LPCSTR Value)
		{
		if(m_Object)
			{
			if(StringHelper::Compare(m_Object->Begin(), Value)==0)
				return;
			Clear();
			}
		if(Value&&Value[0])
			Create(Value);
		}
	VOID Set(LPCWSTR Value)
		{
		if(m_Object)
			{
			if(StringHelper::Compare(m_Object->Begin(), Value)==0)
				return;
			Clear();
			}
		if(Value&&Value[0])
			Create(Value);
		}

	// Operators
	inline Handle<String> operator+(LPCSTR Append)
		{
		if(!m_Object)
			return String::Create(Append);
		return String::Create("%s%s", m_Object->Begin(), Append);
		}
	inline Handle<String> operator+(LPCWSTR Append)
		{
		if(!m_Object)
			return String::Create(Append);
		return String::Create("%s%s", m_Object->Begin(), Append);
		}
	inline Handle<String> operator+(String* Append)
		{
		if(!m_Object)
			return Append;
		if(!Append)
			return m_Object;
		return String::Create("%s%s", m_Object->Begin(), Append->Begin());
		}

private:
	// Con-/Destructors
	inline VOID Create(LPCSTR Value)
		{
		auto str=String::Create(Value);
		_base_t::Create(str);
		}
	inline VOID Create(LPCWSTR Value)
		{
		auto str=String::Create(Value);
		_base_t::Create(str);
		}
	inline VOID Create(LPCSTR Format, VariableArguments const& Arguments)
		{
		auto str=String::Create(Format, Arguments);
		_base_t::Create(str);
		}
};


//==================
// Con-/Destructors
//==================

template <class... _args_t> inline Handle<String> String::Create(LPCSTR Format, _args_t... Arguments)
{
UnknownClass args[]={ Arguments... };
VariableArguments vargs(args, TypeHelper::ArraySize(args));
return Create(Format, vargs);
}
