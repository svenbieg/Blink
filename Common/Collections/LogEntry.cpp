//==============
// LogEntry.cpp
//==============

#include "pch.h"


//=======
// Using
//=======

#include "Timing/Clock.h"
#include "LogEntry.h"

using namespace Timing;


//===========
// Namespace
//===========

namespace Collections {


//========
// Common
//========

SIZE_T LogEntry::ReadFromStream(InputStream* stream)
{
if(!stream)
	return 0;
SIZE_T size=0;
Time=TimePoint::Create();
size+=Time->ReadFromStream(stream);
if(Time->ToSeconds()==0)
	return 0;
Message=Sentence::Create();
size+=Message->ReadFromStream(stream);
return size;
}

SIZE_T LogEntry::WriteToStream(OutputStream* stream)
{
if(!Message||!Time)
	return 0;
SIZE_T size=0;
size+=Time->WriteToStream(stream);
size+=Message->WriteToStream(stream);
return size;
}

//==========================
// Con-/Destructors Private
//==========================

LogEntry::LogEntry()
{}

LogEntry::LogEntry(Handle<TimePoint> time, Handle<Sentence> msg):
Message(msg),
Time(time)
{
if(!Time)
	Time=Clock::Now();
}

}