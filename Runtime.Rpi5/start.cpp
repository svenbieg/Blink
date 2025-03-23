//===========
// start.cpp
//===========

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Memory.h"
#include "Devices/System/Interrupts.h"

using namespace Concurrency;
using namespace Devices::System;


//===========
// Namespace
//===========

extern "C" {


//=============
// Entry-Point
//=============

VOID start()
{
Memory::Initialize();
Interrupts::Initialize();
Scheduler::Initialize();
Scheduler::Begin();
}

VOID start_secondary()
{
#ifdef _DEBUG
BOOL wait=true;
while(wait) {}
#endif
Memory::Enable();
Scheduler::Begin();
}

}