//=====================
// CriticalSection.cpp
//=====================

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#critical-section

#include "CriticalSection.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include <assert.h>

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//==========
// Settings
//==========

constexpr UINT LOCKED=0x80000000;


//========
// Common
//========

VOID CriticalSection::Lock()
{
Interrupts::Disable();
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core!=core);
while(!Cpu::CompareAndSet(&m_Core, 0, core))
	{
	Interrupts::Enable();
	Interrupts::Disable();
	}
Cpu::DataMemoryBarrier();
}

BOOL CriticalSection::TryLock()
{
Interrupts::Disable();
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core!=core);
if(Cpu::CompareAndSet(&m_Core, 0, core))
	{
	Cpu::DataMemoryBarrier();
	return true;
	}
Interrupts::Enable();
return false;
}

VOID CriticalSection::Unlock()
{
UINT core=Cpu::GetId()|LOCKED;
if(m_Core!=core)
	return;
Cpu::DataMemoryBarrier();
Cpu::StoreAndRelease(&m_Core, 0);
Interrupts::Enable();
}

VOID CriticalSection::Yield()
{
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core==core);
Cpu::DataMemoryBarrier();
Cpu::StoreAndRelease(&m_Core, 0);
Interrupts::Enable();
Interrupts::Disable();
while(!Cpu::CompareAndSet(&m_Core, 0, core))
	{
	Interrupts::Enable();
	Interrupts::Disable();
	}
Cpu::DataMemoryBarrier();
}

}