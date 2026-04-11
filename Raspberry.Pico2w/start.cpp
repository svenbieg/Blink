//===========
// start.cpp
//===========

#include "Platform.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "Devices/System/Memory.h"

using namespace Concurrency;
using namespace Devices::System;


//=============
// Entry-Point
//=============

extern "C" VOID start()
{
Memory::Initialize();
Interrupts::Initialize();
Scheduler::Begin();
}

extern "C" VOID start_secondary()
{
BOOL wait=true;
while(wait)
	{
	Cpu::WaitForInterrupt();
	}
Scheduler::Begin();
}
