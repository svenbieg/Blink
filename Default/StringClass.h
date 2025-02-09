//===============
// StringClass.h
//===============

#pragma once


//=======
// Using
//=======

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
	inline UINT64 GetHash()
		{
		if(m_Hash==INVALID_HASH)
			m_Hash=StringHelper::GetHash(m_Buffer);
		return m_Hash;
		}
	static inline UINT64 GetHash(String* String)
		{
		if(!String)
			return 0;
		return String->GetHash();
		}
	inline UINT GetLength()
		{
		if(!m_Length)
			m_Length=StringHelper::Length(m_Buffer);
		return m_Length;
		}
	inline BOOL HasValue()const { return m_Buffer[0]!=0; }
	inline BOOL IsEmpty()const { return m_Buffer[0]==0; }
	template <class... _args_t> inline UINT Scan(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return StringHelper::ScanArgs(m_Buffer, Format, vargs);
		}
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;

	// Comparison
	inline INT Compare(String* String) { return Compare(this, String); }
	static INT Compare(String* String1, String* String2);
	static inline INT Compare(String* String1, LPCSTR String2)
		{
		auto str1=String1? String1->m_Buffer: nullptr;
		return StringHelper::Compare(str1, String2);
		}
	static inline INT Compare(String* String1, LPCWSTR String2)
		{
		auto str1=String1? String1->m_Buffer: nullptr;
		return StringHelper::Compare(str1, String2);
		}

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);

private:
	// Settings
	static constexpr UINT64 INVALID_HASH=(1ULL<<63);

	// Con-/Destructors
	String(LPTSTR Buffer);
	String(LPTSTR Buffer, UINT Size, LPCSTR Value);
	String(LPTSTR Buffer, UINT Size, LPCWSTR Value);
	String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);

	// Common
	LPTSTR m_Buffer;
	UINT64 m_Hash;
	UINT m_Length;
};


//===============
// String-Handle
//===============

template <>
class Handle<String>
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(String* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy) { Handle<Object>::Create(&m_Object, Copy.m_Object); }
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }
	inline Handle(LPCSTR Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(LPCWSTR Value): m_Object(nullptr) { operator=(Value); }

	// Access
	inline operator BOOL()const { return m_Object&&m_Object->HasValue(); }
	inline operator String*()const { return m_Object; }
	inline String* operator->()const { return m_Object; }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return !operator BOOL(); }
	inline BOOL operator==(String* Value)const { return String::Compare(m_Object, Value)==0; }
	inline BOOL operator==(LPCSTR Value)const { return String::Compare(m_Object, Value)==0; }
	inline BOOL operator==(LPCWSTR Value)const { return String::Compare(m_Object, Value)==0; }
	inline BOOL operator!=(nullptr_t)const { return operator BOOL(); }
	inline BOOL operator!=(String* Value)const { return String::Compare(m_Object, Value)!=0; }
	inline BOOL operator!=(LPCSTR Value)const { return String::Compare(m_Object, Value)!=0; }
	inline BOOL operator!=(LPCWSTR Value)const { return String::Compare(m_Object, Value)!=0; }
	inline BOOL operator>(nullptr_t)const { return operator BOOL(); }
	inline BOOL operator>(String* Value)const { return String::Compare(m_Object, Value)>0; }
	inline BOOL operator>(LPCSTR Value)const { return String::Compare(m_Object, Value)>0; }
	inline BOOL operator>(LPCWSTR Value)const { return String::Compare(m_Object, Value)>0; }
	inline BOOL operator>=(nullptr_t)const { return true; }
	inline BOOL operator>=(String* Value)const { return String::Compare(m_Object, Value)>=0; }
	inline BOOL operator>=(LPCSTR Value)const { return String::Compare(m_Object, Value)>=0; }
	inline BOOL operator>=(LPCWSTR Value)const { return String::Compare(m_Object, Value)>=0; }
	inline BOOL operator<(nullptr_t)const { return false; }
	inline BOOL operator<(String* Value)const { return String::Compare(m_Object, Value)<0; }
	inline BOOL operator<(LPCSTR Value)const { return String::Compare(m_Object, Value)<0; }
	inline BOOL operator<(LPCWSTR Value)const { return String::Compare(m_Object, Value)<0; }
	inline BOOL operator<=(nullptr_t)const { return !operator BOOL(); }
	inline BOOL operator<=(String* Value)const { return String::Compare(m_Object, Value)<=0; }
	inline BOOL operator<=(LPCSTR Value)const { return String::Compare(m_Object, Value)<=0; }
	inline BOOL operator<=(LPCWSTR Value)const { return String::Compare(m_Object, Value)<=0; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(String* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	inline Handle& operator=(LPCSTR Value)
		{
		auto str=String::Create(Value);
		return operator=(str);
		}
	inline Handle& operator=(LPCWSTR Value)
		{
		auto str=String::Create(Value);
		return operator=(str);
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
	// Common
	String* m_Object;
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
