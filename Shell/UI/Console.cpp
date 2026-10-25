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

using namespace Collections;
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

VOID Console::Command(Handle<String> cmd)
{
if(!cmd)
	return;
if(Task::IsMainTask())
	{
	HandleCommand(cmd);
	}
else
	{
	DispatchedQueue::Append(this, [this, cmd](){ HandleCommand(cmd); });
	}
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

VOID Console::TaskInfo()
{
WriteLock lock(m_Mutex);
StreamWriter writer(m_SerialPort);
TASK_INFO info[16];
UINT count=TaskMonitor::GetTaskInfo(info, 16);
map<Handle<String>, TASK_INFO*> info_map;
UINT64 total_time=0;
for(UINT u=0; u<count; u++)
	{
	info_map.add(info[u].Name, &info[u]);
	total_time+=info[u].TotalTime;
	}
for(auto const& it: info_map)
	{
	auto name=it.get_key();
	auto info=it.get_value();
	SIZE_T stack_size=info->StackSize;
	SIZE_T stack_used=info->StackUsed;
	UINT64 time=info->TotalTime;
	UINT percent=time*100/total_time;
	writer.Print("%3u%% %-16s %4u/%-4u bytes\n", percent, name, stack_used, stack_size);
	}
writer.Print("\n");
MEMORY_INFO memory;
Memory::GetInfo(&memory);
SIZE_T used=memory.Total-memory.Available;
writer.Print("%u/%u | %u bytes available\n\n", used, memory.Total, memory.Available);
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
m_ConsoleTask=Task::Create(this, &Console::ConsoleTask, "console", 2048);
AddCommand("tasks", this, &Console::TaskInfo);
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