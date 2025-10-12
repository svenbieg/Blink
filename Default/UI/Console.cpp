//=============
// Console.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/StreamWriter.h"
#include "UI/Console.h"

using namespace Concurrency;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace UI {


//========
// Common
//========

Handle<Console> Console::Get()
{
WriteLock lock(s_Mutex);
if(s_Current)
	return s_Current;
auto console=(Console*)operator new(sizeof(Console));
try
	{
	new (console) Console();
	}
catch(Exception e)
	{
	delete console;
	throw e;
	}
s_Current=console;
return console;
}

VOID Console::Print(Handle<String> text)
{
if(!text)
	return;
auto console=Get();
WriteLock lock(console->m_Mutex);
StreamWriter writer(console->m_SerialPort);
writer.Print(text);
writer.Flush();
}


//==========================
// Con-/Destructors Private
//==========================

Console::Console()
{
m_SerialPort=SerialPort::Create();
}

Handle<Console> Console::s_Current;
Mutex Console::s_Mutex;

}