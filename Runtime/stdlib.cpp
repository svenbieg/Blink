//============
// stdlib.cpp
//============

#include "Platform.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/System.h"

using namespace Devices::System;


//=========
// Globals
//=========

VOID* __dso_handle=nullptr;


//========
// Common
//========

extern "C" VOID __assert_func(LPCSTR file, INT line, LPCSTR func, LPCSTR expr)
{
Cpu::Breakpoint();
System::Restart();
}

extern "C" VOID abort()
{
Cpu::Breakpoint();
System::Restart();
}
