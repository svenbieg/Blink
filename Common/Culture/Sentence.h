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
	inline Handle<String> ToString()override { return this->ToString(Language::Current); }
	Handle<String> ToString(LanguageCode Language);
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
class Handle<Culture::Sentence>: public HandleBase<Culture::Sentence>
{
public:
	// Using
	using _base_t=HandleBase<Culture::Sentence>;
	using Sentence=Culture::Sentence;
	using STRING=Resources::Strings::STRING;

	// Con-/Destructors
	using _base_t::_base_t;
	Handle(LPCSTR Value) { Create(new Sentence(Value)); }
	Handle(LPCWSTR Value) { Create(new Sentence(Value)); }
	Handle(STRING const* Value) { Create(new Sentence(Value)); }
	Handle(Handle<String> const& Value) { Create(new Sentence(Value)); }

	// Access
	operator Handle<String>()const { return m_Object->ToString(); }

	// Assignment
	inline Handle& operator=(STRING const* Value) { Set(new Sentence(Value)); return *this; }

	// Comparison
	inline BOOL operator==(Sentence* Value)const override { return Sentence::Compare(m_Object, Value)==0; }
	inline BOOL operator>(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)>0; }
	inline BOOL operator>=(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)>=0; }
	inline BOOL operator<(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)<0; }
	inline BOOL operator<=(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)<=0; }
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