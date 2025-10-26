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

Global<Console> Console::s_Current;

}