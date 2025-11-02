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
m_SerialPort->DataReceived.Add(this, &Console::OnSerialPortDataReceived);
}

Global<Console> Console::s_Current;


//================
// Common Private
//================

VOID Console::OnSerialPortDataReceived()
{
StreamReader reader(m_SerialPort);
while(m_SerialPort->Available())
	{
	TCHAR c=0;
	reader.ReadChar(&c);
	if(CharHelper::IsLineBreak(c))
		{
		auto cmd=m_StringBuilder.ToString();
		if(cmd)
			DispatchedQueue::Append(this, [this, cmd](){ CommandReceived(this, cmd); });
		continue;
		}
	m_StringBuilder.Append(c);
	}
}

}