//=========
// Log.cpp
//=========

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamReader.h"
#include "Storage/Streams/StreamWriter.h"
#include "Log.h"

using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Collections {


//========
// Common
//========

VOID Log::Clear()
{
Entries->Clear();
Changed(this);
}

SIZE_T Log::ReadFromStream(InputStream* stream)
{
if(!stream)
	return 0;
SIZE_T size=0;
StreamReader reader(stream);
while(1)
	{
	Handle<LogEntry> entry=new LogEntry();
	SIZE_T entry_size=entry->ReadFromStream(stream);
	if(!entry_size)
		break;
	Entries->Append(entry);
	size+=entry_size;
	}
return size;
}

VOID Log::Write(Handle<TimePoint> time, Handle<Sentence> msg)
{
Handle<LogEntry> entry=new LogEntry(time, msg);
if(!entry->Time->IsAbsolute())
	entry->Time->Changed.Add(this, &Log::OnTimePointChanged);
Entries->Append(entry);
Changed(this);
}

SIZE_T Log::WriteToStream(OutputStream* stream)
{
SIZE_T size=0;
for(auto it=Entries->Begin(); it->HasCurrent(); it->MoveNext())
	{
	auto entry=it->GetCurrent();
	if(!entry->Time->IsAbsolute())
		break;
	size+=entry->WriteToStream(stream);
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

Log::Log()
{
Entries=EntryList::Create();
}


//================
// Common Private
//================

VOID Log::OnTimePointChanged()
{
Changed(this);
}

}
