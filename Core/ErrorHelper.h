//===============
// ErrorHelper.h
//===============

#pragma once


//=======
// Using
//=======

#include <assert.h>
#include "Concurrency/Scheduler.h"


//========
// Common
//========

#ifdef _DEBUG

VOID DebugPrint(Handle<String> Message);

template <class... _args_t> inline VOID DebugPrint(LPCSTR Format, _args_t... Arguments)
{
Handle<String> msg=new String(Format, Arguments...);
DebugPrint(msg);
}

#else

#define DebugPrint(...)

#endif
