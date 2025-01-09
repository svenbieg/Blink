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


//========
// Common
//========

LanguageCode Language::Current=LanguageCode::None;


//========
// Access
//========

LanguageCode Language::Get()
{
LanguageCode value=m_Value;
Reading(this, value);
return value;
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

SIZE_T Language::WriteToStream(OutputStream* Stream)
{
if(!Stream)
	return sizeof(LanguageCode);
LanguageCode value=Get();
return Stream->Write(&value, sizeof(LanguageCode));
}


//==============
// Modification
//==============

LanguageCode Language::FromString(LPCSTR lng)
{
return LanguageFromString(lng);
}

LanguageCode Language::FromString(LPCWSTR lng)
{
return LanguageFromString(lng);
}

SIZE_T Language::ReadFromStream(InputStream* stream, BOOL notify)
{
if(!stream)
	return sizeof(LanguageCode);
LanguageCode value;
SIZE_T size=stream->Read(&value, sizeof(LanguageCode));
if(size==sizeof(LanguageCode))
	Set(value, notify);
return size;
}

BOOL Language::Set(LanguageCode value, BOOL notify)
{
if(m_Value==value)
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}

}