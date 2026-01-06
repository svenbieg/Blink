//=============
// Console.cpp
//=============

#include "Console.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "Storage/Streams/StreamWriter.h"

using namespace Concurrency;
using namespace Devices::System;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace UI {


//========
// Common
//========

VOID Console::AddCommand(Handle<String> cmd, Function<VOID()> func)
{
WriteLock lock(m_Mutex);
m_Commands.add(cmd, func);
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

Console::Console():
m_This(this)
{
m_SerialPort=SerialPort::Create();
m_SerialPort->SetStreamFormat(StreamFormat::UTF8);
m_SerialPort->DataReceived.Add(this, &Console::OnSerialPortDataReceived);
}


//================
// Common Private
//================

VOID Console::HandleCommand(Handle<String> cmd)
{
ReadLock lock(m_Mutex);
Function<VOID()> func;
BOOL found=m_Commands.try_get(cmd, &func);
lock.Unlock();
if(found)
	{
	func();
	}
else
	{
	CommandReceived(this, cmd);
	}
}

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
			DispatchedQueue::Append(this, [this, cmd](){ HandleCommand(cmd); });
		continue;
		}
	m_StringBuilder.Append(c);
	}
}

}