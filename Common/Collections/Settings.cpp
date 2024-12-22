//==============
// Settings.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Settings.h"

using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Collections {


//==================
// Con-/Destructors
//==================

Settings::Settings()
{
m_SettingsMap=new SettingsMap();
}

Settings::~Settings()
{
for(auto it=m_SettingsMap->First(); it->HasCurrent(); it->MoveNext())
	{
	auto var=it->GetValue();
	var->Changed.Remove(this);
	}
}


//========
// Common
//========

BOOL Settings::Add(Handle<Variable> var)
{
if(!var)
	return false;
if(!m_SettingsMap->Add(var->Name, var))
	return false;
var->Changed.Add(this, &Settings::OnVariableChanged);
return true;
}


SIZE_T Settings::ReadFromStream(InputStream* stream)
{
if(!stream)
	return 0;
SIZE_T size=0;
StreamReader reader(stream);
while(1)
	{
	UINT entry_size=0;
	SIZE_T read=stream->Read(&entry_size, sizeof(UINT));
	if(!read||entry_size==UINT_MAX)
		break;
	read=0;
	auto name=reader.ReadString(&read, "\xFF");
	if(!name)
		break;
	auto var=m_SettingsMap->Get(name);
	if(var)
		read+=var->ReadFromStream(stream);
	if(read<entry_size)
		reader.Skip(entry_size-read);
	size+=sizeof(UINT)+entry_size;
	}
return size;
}

SIZE_T Settings::WriteToStream(OutputStream* stream)
{
SIZE_T size=0;
for(auto it=m_SettingsMap->First(); it->HasCurrent(); it->MoveNext())
	{
	auto var=it->GetValue();
	size+=WriteVariable(stream, var);
	}
return size;
}


//================
// Common Private
//================

VOID Settings::OnVariableChanged()
{
Changed(this);
}

SIZE_T Settings::WriteVariable(OutputStream* stream, Variable* var)
{
StreamWriter writer(nullptr);
UINT entry_size=0;
entry_size+=(UINT)writer.Print(var->Name);
entry_size+=(UINT)writer.PrintChar('\0');
entry_size+=(UINT)var->WriteToStream(nullptr);
if(!stream)
	return sizeof(UINT)+entry_size;
writer.SetStream(stream);
SIZE_T size=0;
size+=stream->Write(&entry_size, sizeof(UINT));
size+=writer.Print(var->Name);
size+=writer.PrintChar('\0');
size+=var->WriteToStream(stream);
return size;
}

}