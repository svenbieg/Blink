//=================
// ErrorHelper.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Collections/StringList.h"
#include "Devices/Serial/SerialPort.h"
#include "Devices/System/Cpu.h"
#include "Storage/Streams/StreamWriter.h"

using namespace Collections;
using namespace Devices::Serial;
using namespace Devices::System;
using namespace Storage::Streams;


//=========
// Globals
//=========

Handle<StringList> g_DebugStrings;


//========
// Common
//========

[[noreturn]] VOID Abort()
{
throw AbortException();
}

[[noreturn]] VOID Abort(LPCSTR file, UINT line, LPCSTR func, LPCSTR expr)
{
DebugPrint("Abort at %s:%s (%u) - %s\n", file, func, line, expr);
Abort();
}

#ifdef _DEBUG

VOID DebugPrint(Handle<String> msg)
{
auto serial=SerialPort::Open();
if(!serial)
	{
	if(!g_DebugStrings)
		g_DebugStrings=new StringList();
	g_DebugStrings->Append(msg);
	return;
	}
StreamWriter writer(serial);
if(g_DebugStrings)
	{
	for(auto it=g_DebugStrings->First(); it->HasCurrent(); it->MoveNext())
		{
		auto str=it->GetCurrent();
		writer.Print(str);
		}
	g_DebugStrings=nullptr;
	}
writer.Print(msg);
}

#endif
