//===============
// StringClass.h
//===============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "Handle.h"
#include "StringHelper.h"
#include <utility>


//======================
// Forward-Declarations
//======================

class String;
template <> class Handle<String>;

class StringBuilder;


//========
// String
//========

class String: public Object
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	friend Handle<String>;
	friend StringBuilder;
	friend StringHelper;

	// Con-/Destructors
	static Handle<String> Create(LPCSTR Value);
	static Handle<String> Create(LPCWSTR Value);
	static Handle<String> Create(UINT Length, LPCSTR Value);
	static Handle<String> Create(UINT Length, LPCWSTR Value);
	static Handle<String> Create(LPCSTR Format, VariableArguments& Arguments);
	template <class... _args_t> static inline Handle<String> Create(LPCSTR Format, _args_t... Arguments);
	static Handle<String> FromInt(INT Value);
	static Handle<String> FromInt64(INT64 Value);
	static Handle<String> ReadFromStream(InputStream* Stream, SIZE_T* Size=nullptr);

	// Access
	inline LPCTSTR Begin()const noexcept { return m_Buffer; }
	inline BOOL Contains(LPCSTR Value, BOOL CaseSensitive=true)noexcept
		{
		return StringHelper::FindString(m_Buffer, Value, nullptr, CaseSensitive);
		}
	inline BOOL Contains(LPCWSTR Value, BOOL CaseSensitive=true)noexcept
		{
		return StringHelper::FindString(m_Buffer, Value, nullptr, CaseSensitive);
		}
	inline UINT Copy(LPSTR Buffer, UINT Size)noexcept { return StringHelper::Copy(Buffer, Size, m_Buffer); }
	inline UINT Copy(LPWSTR Buffer, UINT Size)noexcept { return StringHelper::Copy(Buffer, Size, m_Buffer); }
	inline UINT GetLength()const noexcept { return m_Length; }
	inline BOOL HasValue()const noexcept { return m_Buffer[0]!=0; }
	inline BOOL IsEmpty()const noexcept { return m_Buffer[0]==0; }
	template <class... _args_t> inline UINT Scan(LPCSTR Format, _args_t... Arguments)noexcept
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return StringHelper::Scan(m_Buffer, Format, vargs);
		}
	inline BOOL ToInt(INT* Value)noexcept { return StringHelper::ToInt(m_Buffer, Value); }
	inline BOOL ToInt64(INT64* Value)noexcept { return StringHelper::ToInt64(m_Buffer, Value); }
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;
	SIZE_T WriteToStream(OutputStream* Stream);
	static SIZE_T WriteToStream(String const* String, OutputStream* Stream);

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false)noexcept;

private:
	// Con-/Destructors
	String(LPTSTR Buffer)noexcept;
	String(LPTSTR Buffer, UINT Size, LPCSTR Value)noexcept;
	String(LPTSTR Buffer, UINT Size, LPCWSTR Value)noexcept;
	String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments& Arguments)noexcept;
	static Handle<String> Create(UINT Length, nullptr_t);

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
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	inline Handle()noexcept: m_Object(nullptr) {}
	inline Handle(nullptr_t)noexcept: m_Object(nullptr) {}
	inline Handle(String* Copy)noexcept { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy)noexcept { Handle<Object>::Create(&m_Object, Copy.m_Object); }
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline Handle(LPCSTR Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(LPCWSTR Value): m_Object(nullptr) { operator=(Value); }
	inline ~Handle()noexcept { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator bool()const noexcept { return m_Object&&m_Object->HasValue(); }
	inline operator String*()const noexcept { return m_Object; }
	inline String* operator->()const noexcept { return m_Object; }
	inline SIZE_T WriteToStream(OutputStream* Stream) { return String::WriteToStream(m_Object, Stream); }

	// Comparison
	inline bool operator==(nullptr_t)const noexcept { return !operator bool(); }
	inline bool operator==(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator==(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator==(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator!=(nullptr_t)const noexcept { return operator bool(); }
	inline bool operator!=(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator!=(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator!=(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator>(nullptr_t)const noexcept { return operator bool(); }
	inline bool operator>(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>=(nullptr_t)const noexcept { return true; }
	inline bool operator>=(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator>=(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator>=(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator<(nullptr_t)const noexcept { return false; }
	inline bool operator<(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<=(nullptr_t)const noexcept { return !operator bool(); }
	inline bool operator<=(Handle<String> const& Value)const noexcept { return StringHelper::Compare(m_Object, Value)<=0; }
	inline bool operator<=(LPCSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)<=0; }
	inline bool operator<=(LPCWSTR Value)const noexcept { return StringHelper::Compare(m_Object, Value)<=0; }

	// Assignment
	inline Handle& operator=(nullptr_t)noexcept { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(String* Value)noexcept { Handle<Object>::Set(&m_Object, Value); return *this; }
	inline Handle& operator=(Handle<String> const& Value)noexcept { return operator=(Value.m_Object); }
	inline Handle& operator=(LPCSTR Value) { return operator=(String::Create(Value)); }
	inline Handle& operator=(LPCWSTR Value) { return operator=(String::Create(Value)); }
	SIZE_T ReadFromStream(InputStream* Stream)
		{
		SIZE_T size=0;
		auto str=String::ReadFromStream(Stream, &size);
		operator=(std::move(str));
		return size;
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
VariableArgument args[]={ Arguments... };
VariableArguments vargs(args, TypeHelper::ArraySize(args));
return Create(Format, vargs);
}
