//============
// LogEntry.h
//============

#pragma once


//=======
// Using
//=======

#include "Culture/Sentence.h"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "Timing/TimePoint.h"


//===========
// Namespace
//===========

namespace Collections {


//===========
// Log-Entry
//===========

class LogEntry: public Object
{
private:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;
	using Sentence=Culture::Sentence;
	using TimePoint=Timing::TimePoint;

public:
	// Con-/Destructors
	LogEntry();
	LogEntry(Handle<TimePoint> Time, Handle<Sentence> Message);

	// Common
	SIZE_T ReadFromStream(InputStream* Stream);
	SIZE_T WriteToStream(OutputStream* Stream);
	Handle<Sentence> Message;
	Handle<TimePoint> Time;
};

}