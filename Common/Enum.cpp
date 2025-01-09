//==========
// Enum.cpp
//==========

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Enum.h"

using namespace Culture;
using namespace Storage::Streams;


//========
// Access
//========

Handle<EnumIterator> Enum::Begin()
{
auto it=new EnumIterator(this);
it->Begin();
return it;
}

Handle<String> Enum::ToString(LanguageCode lng)
{
assert(m_Value);
return m_Value->Begin(lng);
}

SIZE_T Enum::WriteToStream(OutputStream* stream)
{
SIZE_T size=0;
StreamWriter writer(stream);
size+=writer.Print(m_Value->Begin(LanguageCode::None));
size+=writer.PrintChar('\0');
return size;
}


//==============
// Modification
//==============

VOID Enum::Add(Handle<Sentence> value)
{
m_Values.set(value);
if(!m_Value)
	m_Value=value;
}

BOOL Enum::FromString(Handle<String> str, BOOL notify)
{
if(!str)
	return false;
for(auto it=m_Values.cbegin(); it.has_current(); it.move_next())
	{
	auto sentence=it.get_current();
	if(sentence->Compare(str->Begin())==0)
		return Set(sentence);
	}
return false;
}

SIZE_T Enum::ReadFromStream(InputStream* stream, BOOL notify)
{
SIZE_T size=0;
StreamReader reader(stream);
auto str=reader.ReadString(&size);
FromString(str, notify);
return size;
}

BOOL Enum::Set(Handle<Sentence> value, BOOL notify)
{
if(m_Value==value)
	return true;
if(!m_Values.contains(value))
	return false;
m_Value=value;
if(notify)
	Changed(this);
return true;
}
