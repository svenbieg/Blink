//=================
// ErrorHelper.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Collections/StringList.h"

using namespace Collections;


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
if(!g_DebugStrings)
	g_DebugStrings=new StringList();
g_DebugStrings->Append(msg);
}

#endif
