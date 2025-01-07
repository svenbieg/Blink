//==============
// Sentence.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Sentence.h"

using namespace Resources::Strings;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Culture {


//========
// Common
//========

LPCTSTR Sentence::Begin(LanguageCode lng)
{
auto str=ToString(lng);
return str? str->Begin(): nullptr;
}

INT Sentence::Compare(LPCSTR value)const
{
for(auto it: m_Strings)
	{
	auto str=it.get_value();
	if(StringHelper::Compare(str->Begin(), value, 0, false)==0)
		return 0;
	}
return -1;
}

INT Sentence::Compare(LPCWSTR value)const
{
for(auto it: m_Strings)
	{
	auto str=it.get_value();
	if(StringHelper::Compare(str->Begin(), value, 0, false)==0)
		return 0;
	}
return -1;
}

INT Sentence::Compare(STRING const* str)const
{
if(m_String==str)
	return 0;
if(!str)
	return -1;
while(str->Language!=LanguageCode::None)
	{
	auto str1=m_Strings.get(str->Language);
	if(!str1)
		continue;
	if(StringHelper::Compare(str1->Begin(), str->Value, 0, false)==0)
		return 0;
	}
return -1;
}

INT Sentence::Compare(STRING const* str, LPCTSTR value)
{
if(!str)
	{
	if(!value)
		return 0;
	return -1;
	}
if(!value)
	return -1;
while(str->Language!=LanguageCode::None)
	{
	if(StringHelper::Compare(str->Value, value, 0, false)==0)
		return 0;
	str++;
	}
return -1;
}

INT Sentence::Compare(Sentence const* sentence1, Sentence const* sentence2)
{
if(!sentence1||!sentence2)
	return -1;
if(sentence1->m_String==sentence2->m_String)
	return 0;
for(auto it: sentence1->m_Strings)
	{
	auto lng=it.get_key();
	auto str2=sentence2->m_Strings.get(lng);
	if(!str2)
		continue;
	auto str1=it.get_value();
	return StringHelper::Compare(str1->Begin(), str2->Begin(), 0, false);
	}
return -1;
}

SIZE_T Sentence::ReadFromStream(InputStream* stream)
{
StreamReader reader(stream);
SIZE_T size=0;
while(1)
	{
	SIZE_T lng_len=0;
	auto lng_str=reader.ReadString(&lng_len, "\t");
	size+=lng_len;
	if(!lng_str)
		break;
	auto lng=Language::FromString(lng_str->Begin());
	if(lng==LanguageCode::None)
		break;
	SIZE_T str_len=0;
	auto str=reader.ReadString(&str_len, "\n");
	if(!str_len)
		break;
	size+=str_len;
	m_Strings.add(lng, str);
	}
return size;
}

Handle<String> Sentence::ToString(LanguageCode lng)
{
auto str=m_Strings.get(lng);
if(str)
	return str;
str=m_Strings.get(Language::Current);
if(str)
	return str;
if(!m_Strings)
	return nullptr;
return m_Strings.get_at(0).get_value();
}

LPCWSTR Sentence::Translate(STRING const* str, LanguageCode lng)
{
if(!str)
	return nullptr;
while(str)
	{
	if(str->Language==LanguageCode::None)
		break;
	if(str->Language==lng)
		return str->Value;
	str++;
	}
return nullptr;
}

SIZE_T Sentence::WriteToStream(OutputStream* stream)const
{
StreamWriter writer(stream);
SIZE_T size=0;
for(auto it: m_Strings)
	{
	auto lng_str=Language::ToStringCode(it.get_key());
	if(!lng_str)
		break;
	size+=writer.Print(lng_str);
	size+=writer.Print("\t");
	size+=writer.Print(it.get_value());
	size+=writer.Print("\n");
	}
size+=writer.Print("\0");
return size;
}


//==========================
// Con-/Destructors Private
//==========================

Sentence::Sentence():
m_String(nullptr)
{}

Sentence::Sentence(LPCSTR str):
m_String(nullptr)
{
if(!str)
	return;
m_Strings.add(Language::Current, str);
}

Sentence::Sentence(LPCWSTR str):
m_String(nullptr)
{
if(!str)
	return;
m_Strings.add(Language::Current, str);
}

Sentence::Sentence(STRING const* str):
m_String(str)
{
if(!str)
	return;
while(str->Language!=LanguageCode::None)
	{
	m_Strings.add(str->Language, str->Value);
	str++;
	}
}

Sentence::Sentence(Handle<String> const& str):
m_String(nullptr)
{
if(!str)
	return;
m_Strings.add(Language::Current, str);
}

}