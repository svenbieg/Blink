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

#ifdef _DEBUG

VOID DebugPrint(Handle<String> msg)
{
if(!g_DebugStrings)
	g_DebugStrings=new StringList();
g_DebugStrings->Append(msg);
}

#endif
