//============
// Sentence.h
//============

#pragma once


//=======
// Using
//=======

#include "Collections/map.hpp"
#include "Resources/Strings/String.h"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Culture {


//==========
// Sentence
//==========

class Sentence: public Object
{
public:
	// Friends
	friend class Handle<Sentence>;

	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;
	using STRING=Resources::Strings::STRING;

	// Con-/Destructors
	static Handle<Sentence> Create();
	static Handle<Sentence> Create(LPCSTR Value);
	static Handle<Sentence> Create(LPCWSTR Value);
	static Handle<Sentence> Create(STRING const* Value);
	static Handle<Sentence> Create(Handle<String> const& Value);

	// Common
	LPCTSTR Begin(LanguageCode Language=Language::Current);
	INT Compare(LPCSTR Value)const;
	INT Compare(LPCWSTR Value)const;
	INT Compare(STRING const* Value)const;
	static INT Compare(STRING const* String, LPCTSTR Value);
	static INT Compare(Sentence const* Sentence1, Sentence const* Sentence2);
	SIZE_T ReadFromStream(InputStream* Stream);
	Handle<String> ToString(LanguageCode Language=LanguageCode::None)override;
	static LPCWSTR Translate(STRING const* Value, LanguageCode Language=Language::Current);
	SIZE_T WriteToStream(OutputStream* Stream)const;

private:
	// Con-/Destructors
	Sentence();
	Sentence(LPCSTR Value);
	Sentence(LPCWSTR Value);
	Sentence(STRING const* Value);
	Sentence(Handle<String> const& Value);

	// Common
	STRING const* m_String;
	Collections::map<LanguageCode, Handle<String>> m_Strings;
};

}


//========
// Handle
//========

template <>
class Handle<Culture::Sentence>
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
	inline Handle(Sentence* Copy) { Handle<Object>::Create(&m_Object, Copy); }
	inline Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	inline Handle(Handle&& Move)noexcept: m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	inline Handle(LPCSTR Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(LPCWSTR Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(STRING const* Value): m_Object(nullptr) { operator=(Value); }
	inline Handle(Handle<String> const& Value): m_Object(nullptr) { operator=(Value); }
	inline ~Handle() { Handle<Object>::Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator Sentence*()const { return m_Object; }
	inline Sentence* operator->()const { return m_Object; }
	operator Handle<String>()const { return m_Object? m_Object->ToString(): nullptr; }

	// Comparison
	inline BOOL operator==(Sentence* Value)const { return Sentence::Compare(m_Object, Value)==0; }
	inline BOOL operator!=(Sentence* Value)const { return Sentence::Compare(m_Object, Value)!=0; }
	inline BOOL operator>(Sentence* Value)const { return Sentence::Compare(m_Object, Value)>0; }
	inline BOOL operator>=(Sentence* Value)const { return Sentence::Compare(m_Object, Value)>=0; }
	inline BOOL operator<(Sentence* Value)const { return Sentence::Compare(m_Object, Value)<0; }
	inline BOOL operator<=(Sentence* Value)const { return Sentence::Compare(m_Object, Value)<=0; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Handle<Object>::Clear(&m_Object); return *this; }
	inline Handle& operator=(Sentence* Copy) { Handle<Object>::Set(&m_Object, Copy); return *this; }
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	inline Handle& operator=(LPCSTR Value)
		{
		auto value=Sentence::Create(Value);
		return operator=(value);
		}
	inline Handle& operator=(LPCWSTR Value)
		{
		auto value=Sentence::Create(Value);
		return operator=(value);
		}
	inline Handle& operator=(STRING const* Value)
		{
		auto value=Sentence::Create(Value);
		return operator=(value);
		}

private:
	// Common
	Sentence* m_Object;
};


//==================
// Con-/Destructors
//==================

namespace Culture {

inline Handle<Sentence> Sentence::Create()
{
return new Sentence();
}

inline Handle<Sentence> Sentence::Create(LPCSTR Value)
{
return new Sentence(Value);
}

inline Handle<Sentence> Sentence::Create(LPCWSTR Value)
{
return new Sentence(Value);
}

inline Handle<Sentence> Sentence::Create(STRING const* Value)
{
return new Sentence(Value);
}

inline Handle<Sentence> Sentence::Create(Handle<String> const& Value)
{
return new Sentence(Value);
}

}