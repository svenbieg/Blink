//=====================
// CriticalSection.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"
#include "CriticalSection.h"

using namespace Devices::System;


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Con-/Destructors
//==================

CriticalSection::CriticalSection():
m_Core(CPU_COUNT),
m_LockCount(0)
{}


//========
// Common
//========

VOID CriticalSection::Lock()
{
Interrupts::Disable();
UINT core=Cpu::GetId();
if(m_Core==core)
	{
	m_LockCount++;
	return;
	}
while(!Cpu::CompareAndSet(&m_Core, CPU_COUNT, core))
	{
	Interrupts::Enable();
	Cpu::WaitForEvent();
	Interrupts::Disable();
	}
m_LockCount++;
Cpu::DataSyncBarrier();
}

BOOL CriticalSection::TryLock()
{
Interrupts::Disable();
UINT core=Cpu::GetId();
if(m_Core==core)
	{
	m_LockCount++;
	return true;
	}
if(Cpu::CompareAndSet(&m_Core, CPU_COUNT, core))
	{
	m_LockCount++;
	Cpu::DataSyncBarrier();
	return true;
	}
Interrupts::Enable();
return false;
}

VOID CriticalSection::Unlock()
{
UINT core=Cpu::GetId();
assert(m_Core==core);
if(--m_LockCount==0)
	{
	Cpu::StoreAndRelease(&m_Core, CPU_COUNT);
	Cpu::DataStoreBarrier();
	Cpu::SetEvent();
	}
Interrupts::Enable();
}

}
