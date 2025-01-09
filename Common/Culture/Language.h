//============
// Language.h
//============

#pragma once


//=======
// Using
//=======

#include "LanguageCode.h"
#include "Variable.h"


//===========
// Namespace
//===========

namespace Culture {


//==========
// Language
//==========

class Language: public Variable
{
public:
	// Con-/Destructors
	static inline Handle<Language> Create(Handle<String> Name=nullptr, LanguageCode Value=LanguageCode::None)
		{
		return new Language(Name, Value);
		}

	// Common
	static LanguageCode Current;

	// Access
	LanguageCode Get();
	static inline LanguageCode Get(Language* Value) { return Value? Value->Get(): LanguageCode::None; }
	inline Handle<String> GetName()const override { return m_Name; }
	Event<Variable, LanguageCode&> Reading;
	inline Handle<String> ToString(LanguageCode Language=LanguageCode::None)override { return ToString(Get(), Language); }
	static Handle<String> ToString(LanguageCode Language, LanguageCode ToLanguage=Current);
	inline Handle<String> ToStringCode() { return ToStringCode(Get()); }
	static Handle<String> ToStringCode(LanguageCode Language);
	SIZE_T WriteToStream(OutputStream* Stream)override;

	// Common
	static LanguageCode FromString(LPCSTR Value);
	static LanguageCode FromString(LPCWSTR Value);
	static inline LanguageCode FromString(Handle<String> Value) { return FromString(Value? Value->Begin(): nullptr); }
	SIZE_T ReadFromStream(InputStream* Stream, BOOL Notify=true)override;
	BOOL Set(LanguageCode Value, BOOL Notify=true);

private:
	// Con-/Destructors
	Language(Handle<String> Name, LanguageCode Value): m_Name(Name), m_Value(Value) {}

	// Common
	Handle<String> m_Name;
	LanguageCode m_Value;
};

}