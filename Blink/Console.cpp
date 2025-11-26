//=============
// Console.cpp
//=============

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "Storage/Streams/StreamWriter.h"
#include "Application.h"
#include "Console.h"

using namespace Concurrency;
using namespace Devices::System;
using namespace Storage::Streams;


//===========
// Namespace
//===========

namespace Blink {


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
m_SerialPort->SetFormat(StreamFormat::UTF8);
m_SerialPort->DataReceived.Add(this, &Console::OnSerialPortDataReceived);
m_Commands.add("off", []()
	{
	Console::Print("Led off\n");
	System::Led(false);
	});
m_Commands.add("on", []()
	{
	Console::Print("Led on\n");
	System::Led(true);
	});
m_Commands.add("start", []()
	{
	auto app=Application::Get();
	app->StartBlinking();
	});
m_Commands.add("stop", []()
	{
	auto app=Application::Get();
	app->StopBlinking();
	});
auto console=Console::Get();
CommandReceived.Add(this, &Console::OnCommandReceived);
console->Print("Blink commands:");
for(auto cmd: m_Commands)
	{
	auto name=cmd.get_key();
	console->Print(" ");
	console->Print(name);
	}
console->Print("\n");
}

Global<Console> Console::s_Current;


//================
// Common Private
//================

VOID Console::OnCommandReceived(Handle<String> cmd)
{
auto func=m_Commands.get(cmd);
if(func)
	func();
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
			DispatchedQueue::Append(this, [this, cmd](){ CommandReceived(this, cmd); });
		continue;
		}
	m_StringBuilder.Append(c);
	}
}

}