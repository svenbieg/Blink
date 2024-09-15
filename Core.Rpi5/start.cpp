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


//=============
// Entry-Point
//=============

extern "C" VOID start()
{
Memory::Initialize();
Interrupts::Initialize();
Scheduler::Initialize();
//for(UINT u=1; u<CPU_COUNT; u++)
//	Cpu::PowerOn(u);
Scheduler::Begin();
}

extern "C" VOID start_secondary()
{
Memory::Enable();
Scheduler::Begin();
}
