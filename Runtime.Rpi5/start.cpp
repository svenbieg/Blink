//===========
// start.cpp
//===========

#include "Platform.h"


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
Scheduler::Begin();
}

VOID start_secondary()
{
while(1)
	Cpu::WaitForInterrupt();
Interrupts::InitializeSecondary();
Scheduler::Begin();
}

}