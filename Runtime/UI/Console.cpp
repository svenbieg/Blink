//=============
// Console.cpp
//=============

#include "Console.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Devices/System/System.h"
#include "Storage/Streams/StreamReader.h"
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

VOID Console::AddCommand(Handle<String> cmd, VOID (*proc)())
{
auto handler=CallableFunction<VOID>::Create(proc);
WriteLock lock(m_Mutex);
m_Commands.add(cmd, handler);
}

VOID Console::Print(LPCSTR text)
{
if(!text)
	return;
auto console=Create();
WriteLock lock(console->m_Mutex);
StreamWriter writer(console->m_SerialPort);
writer.Print(text);
writer.Flush();
}

VOID Console::Print(Handle<String> text)
{
if(!text)
	return;
auto console=Create();
WriteLock lock(console->m_Mutex);
StreamWriter writer(console->m_SerialPort);
writer.Print(text);
writer.Flush();
}


//===============
// Output-Stream
//===============

VOID Console::Flush()
{
m_SerialPort->Flush();
}

SIZE_T Console::Write(VOID const* buf, SIZE_T size)
{
return m_SerialPort->Write(buf, size);
}


//==========================
// Con-/Destructors Private
//==========================

Console::Console():
m_This(this)
{
m_SerialPort=SerialPort::Create();
m_ConsoleTask=Task::Create(this, &Console::ConsoleTask, "console", 1024);
}


//================
// Common Private
//================

VOID Console::ConsoleTask()
{
StreamReader reader(m_SerialPort);
auto task=Task::Get();
while(!task->Cancelled)
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

VOID Console::HandleCommand(Handle<String> cmd)
{
ReadLock lock(m_Mutex);
Handle<ConsoleHandler> handler;
BOOL found=m_Commands.try_get(cmd, &handler);
lock.Unlock();
if(found)
	{
	handler->Call();
	}
else
	{
	CommandReceived(this, cmd);
	}
}

}