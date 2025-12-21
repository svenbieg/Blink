//===============
// StringClass.h
//===============

#pragma once


//=======
// Using
//=======

#include "Storage/Encoding/Dwarf.h"
#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Handle.h"
#include "StringHelper.h"


//======================
// Forward-Declarations
//======================

class String;
template <> class Handle<String>;

class StringBuilder;

namespace Storage
{
namespace Streams
	{
	class StreamReader;
	}
}


//========
// String
//========

class String: public Object
{
public:
	// Friends
	friend Handle<String>;
	friend StringBuilder;
	friend StringHelper;

	// Con-/Destructors
	static Handle<String> Create(LPCSTR Value);
	static Handle<String> Create(LPCWSTR Value);
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
	inline UINT Copy(LPSTR Buffer, UINT Size) { return StringHelper::Copy(Buffer, Size, m_Buffer); }
	inline UINT Copy(LPWSTR Buffer, UINT Size) { return StringHelper::Copy(Buffer, Size, m_Buffer); }
	inline UINT GetLength()const { return m_Length; }
	inline BOOL HasValue()const { return m_Buffer[0]!=0; }
	inline BOOL IsEmpty()const { return m_Buffer[0]==0; }
	template <class... _args_t> inline UINT Scan(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return StringHelper::ScanArgs(m_Buffer, Format, vargs);
		}
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;

	// Operators
	Handle<String> Replace(LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);

private:
	// Con-/Destructors
	String(LPTSTR Buffer);
	String(LPTSTR Buffer, UINT Size, LPCSTR Value);
	String(LPTSTR Buffer, UINT Size, LPCWSTR Value);
	String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);
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
	using Dwarf=Storage::Encoding::Dwarf;
	using IInputStream=Storage::Streams::IInputStream;
	using IOutputStream=Storage::Streams::IOutputStream;
	using StreamReader=Storage::Streams::StreamReader;
	using StreamWriter=Storage::Streams::StreamWriter;

	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(String* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy) { Handle<Object>::Create(&m_Object, Copy.m_Object); }
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline Handle(LPCSTR Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(LPCWSTR Value): m_Object(nullptr) { operator=(Value); }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator bool()const { return m_Object&&m_Object->HasValue(); }
	inline operator String*()const { return m_Object; }
	inline String* operator->()const { return m_Object; }
	SIZE_T WriteToStream(IOutputStream* Stream)
		{
		if(!m_Object)
			return Dwarf::WriteUnsigned(Stream, 0U);
		StreamWriter writer(Stream);
		SIZE_T size=0;
		auto len=m_Object->m_Length;
		size+=Dwarf::WriteUnsigned(Stream, len);
		size+=writer.Print(len, m_Object->m_Buffer);
		return size;
		}

	// Comparison
	inline bool operator==(nullptr_t)const { return !operator bool(); }
	inline bool operator==(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator==(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator==(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)==0; }
	inline bool operator!=(nullptr_t)const { return operator bool(); }
	inline bool operator!=(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator!=(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator!=(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)!=0; }
	inline bool operator>(nullptr_t)const { return operator bool(); }
	inline bool operator>(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)>0; }
	inline bool operator>=(nullptr_t)const { return true; }
	inline bool operator>=(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator>=(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator>=(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)>=0; }
	inline bool operator<(nullptr_t)const { return false; }
	inline bool operator<(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)<0; }
	inline bool operator<=(nullptr_t)const { return !operator bool(); }
	inline bool operator<=(Handle<String> const& Value)const { return StringHelper::Compare(m_Object, Value)<=0; }
	inline bool operator<=(LPCSTR Value)const { return StringHelper::Compare(m_Object, Value)<=0; }
	inline bool operator<=(LPCWSTR Value)const { return StringHelper::Compare(m_Object, Value)<=0; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(String* Value) { Handle<Object>::Set(&m_Object, Value); return *this; }
	inline Handle& operator=(Handle<String> const& Value) { return operator=(Value.m_Object); }
	inline Handle& operator=(LPCSTR Value) { return operator=(String::Create(Value)); }
	inline Handle& operator=(LPCWSTR Value) { return operator=(String::Create(Value)); }
	SIZE_T ReadFromStream(IInputStream* Stream)
		{
		StreamReader reader(Stream);
		SIZE_T size=0;
		UINT len=0;
		size+=Dwarf::ReadUnsigned(Stream, &len);
		if(!len)
			return size;
		auto str=String::Create(len, nullptr);
		auto buf=str->m_Buffer;
		for(UINT u=0; u<len; u++)
			size+=reader.ReadChar(&buf[u]);
		buf[len]=0;
		str->m_Hash=StringHelper::GetHash(buf);
		str->m_Length=len;
		operator=(str);
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
UnknownClass args[]={ Arguments... };
VariableArguments vargs(args, TypeHelper::ArraySize(args));
return Create(Format, vargs);
}
