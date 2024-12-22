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

using namespace Concurrency;
using namespace Culture;
using namespace Storage::Streams;


//==================
// Con-/Destructors
//==================

Enum::Enum(Handle<String> name):
Variable(name)
{}


//========
// Access
//========

Handle<EnumIterator> Enum::First()
{
auto it=new EnumIterator(this);
it->First();
return it;
}

Handle<Sentence> Enum::Get()
{
SharedLock lock(m_Mutex);
return m_Value;
}

Handle<String> Enum::ToString(LanguageCode lng)
{
SharedLock lock(m_Mutex);
return m_Value->Begin(lng);
}

SIZE_T Enum::WriteToStream(OutputStream* stream)
{
SharedLock lock(m_Mutex);
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
ScopedLock lock(m_Mutex);
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
ScopedLock lock(m_Mutex);
if(!m_Values.contains(value))
	return false;
if(m_Value==value)
	return true;
m_Value=value;
lock.Unlock();
if(notify)
	Changed(this);
return true;
}


//===========================
// Iterator Con-/Destructors
//===========================

EnumIterator::EnumIterator(Handle<Enum> henum):
m_Enum(henum),
m_It(&henum->m_Values)
{
m_Enum->m_Mutex.Lock();
}

EnumIterator::~EnumIterator()
{
m_Enum->m_Mutex.Unlock();
}
