//===========
// Flags.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Collections/StringList.h"
#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Flags.h"

using namespace Collections;
using namespace Storage::Streams;


//========
// Access
//========

BOOL Flags::Get(Handle<String> flag)
{
SharedLock lock(m_Mutex);
return m_Flags.contains(flag);
}

Handle<String> Flags::ToString(LanguageCode lng)
{
SharedLock lock(m_Mutex);
Handle<String> flags;
UINT count=0;
for(auto it=m_Flags.cbegin(); it.has_current(); it.move_next())
	{
	if(count>0)
		flags=flags+"|";
	auto flag=it.get_current();
	flags=flags+flag;
	count++;
	}
return flags;
}

SIZE_T Flags::WriteToStream(OutputStream* stream)
{
auto flags=Variable::ToString();
SIZE_T size=0;
StreamWriter writer(stream);
size+=writer.Print(flags);
size+=writer.PrintChar('\0');
return size;
}


//==============
// Modification
//==============

VOID Flags::Clear(BOOL notify)
{
ScopedLock lock(m_Mutex);
if(m_Flags.get_count()==0)
	return;
m_Flags.clear();
if(notify)
	Changed(this);
}

VOID Flags::Clear(Handle<String> flag, BOOL notify)
{
ScopedLock lock(m_Mutex);
if(!m_Flags.contains(flag))
	return;
m_Flags.remove(flag);
if(notify)
	Changed(this);
}

BOOL Flags::FromString(Handle<String> str, BOOL notify)
{
if(!str)
	return false;
if(str=="0")
	{
	Clear(notify);
	return true;
	}
ScopedLock lock(m_Mutex);
auto list=StringList::Create(str);
BOOL changed=false;
for(auto it=list->Begin(); it->HasCurrent(); it->MoveNext())
	{
	auto flag=it->GetCurrent();
	auto p=flag->Begin();
	if(CharHelper::Compare(p[0], '!')==0)
		{
		auto clear=String::Create(&p[1]);
		if(m_Flags.contains(clear))
			{
			m_Flags.remove(clear);
			changed=true;
			}
		}
	else
		{
		if(!m_Flags.contains(flag))
			{
			m_Flags.set(flag);
			changed=true;
			}
		}
	}
lock.Unlock();
if(changed&&notify)
	Changed(this);
return true;
}

SIZE_T Flags::ReadFromStream(InputStream* stream, BOOL notify)
{
SIZE_T size=0;
StreamReader reader(stream);
auto str=reader.ReadString(&size);
FromString(str, notify);
return size;
}

VOID Flags::Set(Handle<String> flag, BOOL notify)
{
ScopedLock lock(m_Mutex);
if(m_Flags.contains(flag))
	return;
m_Flags.set(flag);
lock.Unlock();
if(notify)
	Changed(this);
}


//==========================
// Con-/Destructors Private
//==========================

Flags::Flags(Handle<String> name):
Variable(name)
{}
