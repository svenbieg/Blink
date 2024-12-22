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
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;
	using STRING=Resources::Strings::STRING;

	// Con-/Destructors
	Sentence();
	Sentence(LPCSTR Value);
	Sentence(STRING const* Value);

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
	// Common
	STRING const* m_String;
	Collections::map<LanguageCode, Handle<String>> m_Strings;
};

}


//========
// Handle
//========

template <>
class Handle<Culture::Sentence>: public ::Details::HandleBase<Culture::Sentence>
{
public:
	// Using
	using _base_t=::Details::HandleBase<Culture::Sentence>;
	using _base_t::_base_t;
	using Sentence=Culture::Sentence;
	using STRING=Resources::Strings::STRING;

	// Con-/Destructors
	Handle(LPCSTR Value) { Create(new Sentence(Value)); }
	Handle(STRING const* Value) { Create(new Sentence(Value)); }

	// Assignment
	inline Handle& operator=(STRING const* Value) { Set(new Sentence(Value)); return *this; }

	// Comparison
	inline BOOL operator==(Sentence* Value)const override { return Sentence::Compare(m_Object, Value)==0; }
	inline BOOL operator>(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)>0; }
	inline BOOL operator>=(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)>=0; }
	inline BOOL operator<(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)<0; }
	inline BOOL operator<=(Handle<Sentence> const& Value)const { return Sentence::Compare(m_Object, Value.m_Object)<=0; }
};
