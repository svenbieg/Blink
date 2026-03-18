//=====================
// CriticalSection.cpp
//=====================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
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

const UINT LOCKED=0x80000000;


//========
// Common
//========

VOID CriticalSection::Lock()noexcept
{
Interrupts::Disable();
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core!=core); // Deadlock
while(!Cpu::CompareAndSet(&m_Core, 0, core))
	{
	Interrupts::Enable();
	Interrupts::Disable();
	}
}

BOOL CriticalSection::TryLock()noexcept
{
Interrupts::Disable();
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core!=core); // Deadlock
if(Cpu::CompareAndSet(&m_Core, 0, core))
	return true;
Interrupts::Enable();
return false;
}

VOID CriticalSection::Unlock()noexcept
{
UINT core=Cpu::GetId()|LOCKED;
if(m_Core!=core)
	return;
Cpu::StoreAndRelease(&m_Core, 0);
Interrupts::Enable();
}

VOID CriticalSection::Yield()noexcept
{
UINT core=Cpu::GetId()|LOCKED;
assert(m_Core==core);
Cpu::StoreAndRelease(&m_Core, 0);
Interrupts::Enable();
Interrupts::Disable();
while(!Cpu::CompareAndSet(&m_Core, 0, core))
	{
	Interrupts::Enable();
	Interrupts::Disable();
	}
}

}