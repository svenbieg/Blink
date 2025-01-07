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

class Language: public TypedVariable<LanguageCode>
{
public:
	// Con-/Destructors
	static inline Handle<Language> Create(Handle<String> Name=nullptr, LanguageCode Value=LanguageCode::None)
		{
		return new Language(Name, Value);
		}

	// Common
	static LanguageCode Current;
	static LanguageCode FromString(LPCSTR Language);
	static LanguageCode FromString(LPCWSTR Language);
	static Handle<String> ToString(LanguageCode Language=Current, LanguageCode ToLanguage=Current);
	static Handle<String> ToStringCode(LanguageCode Language=Current);
	Handle<String> ToString()override;
	Handle<String> ToString(LanguageCode ToLanguage);
	Handle<String> ToStringCode();

private:
	// Con-/Destructors
	Language(Handle<String> Name, LanguageCode Language);
};

}