//==============
// Language.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Resources/Strings/Language.h"
#include "Language.h"
#include "Sentence.h"

using namespace Resources::Strings;


//===========
// Namespace
//===========

namespace Culture {


//===========
// Templates
//===========

template <class _char_t> LanguageCode LanguageFromString(_char_t const* lng)
{
auto str=STR_LANGUAGE_CODE;
while(str->Language!=LanguageCode::None)
	{
	if(StringHelper::Compare(str->Value, lng, 2, false)==0)
		return str->Language;
	str++;
	}
return LanguageCode::None;
}


//=============
// Translation
//=============

typedef struct
{
LNG Language;
STRING const* String;
}TRANSLATION;

constexpr TRANSLATION TRANS_LANGUAGE[]=
	{
	{ LNG::DE, STR_GERMAN },
	{ LNG::EN, STR_ENGLISH },
	{ LNG::None, nullptr }
	};


//==================
// Con-/Destructors
//==================

Language::Language(Handle<String> name, LanguageCode lng):
TypedVariable(name, lng)
{}


//========
// Common
//========

LanguageCode Language::Current=LanguageCode::None;

LanguageCode Language::FromString(LPCSTR lng)
{
return LanguageFromString(lng);
}

LanguageCode Language::FromString(LPCWSTR lng)
{
return LanguageFromString(lng);
}

Handle<String> Language::ToString(LanguageCode lng, LanguageCode lng_to)
{
auto trans=TRANS_LANGUAGE;
while(trans->Language!=LanguageCode::None)
	{
	if(trans->Language==lng_to)
		return Sentence::Translate(trans->String, lng);
	}
return nullptr;
}

Handle<String> Language::ToStringCode(LanguageCode lng)
{
return Sentence::Translate(STR_LANGUAGE_CODE, lng);
}

Handle<String> Language::ToString()
{
return ToString(m_Value, Current);
}

Handle<String> Language::ToString(LanguageCode lng_to)
{
return ToString(m_Value, lng_to);
}

Handle<String> Language::ToStringCode()
{
return ToStringCode(m_Value);
}

}