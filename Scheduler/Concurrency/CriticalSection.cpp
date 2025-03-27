//=====================
// CriticalSection.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include <assert.h>
#include "Concurrency/CriticalSection.h"
#include "Concurrency/SpinLock.h"
#include "Devices/System/Cpu.h"
#include "Devices/System/Interrupts.h"

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
if(m_Core!=core)
	return;
if(--m_LockCount==0)
	{
	Cpu::DataStoreBarrier();
	Cpu::StoreAndRelease(&m_Core, CPU_COUNT);
	}
Interrupts::Enable();
}

VOID CriticalSection::Yield()
{
UINT core=Cpu::GetId();
assert(m_Core==core);
assert(m_LockCount==1);
m_LockCount--;
Cpu::DataStoreBarrier();
Cpu::StoreAndRelease(&m_Core, CPU_COUNT);
Interrupts::Enable();
Interrupts::Disable();
while(!Cpu::CompareAndSet(&m_Core, CPU_COUNT, core))
	{
	Interrupts::Enable();
	Interrupts::Disable();
	}
m_LockCount++;
Cpu::DataSyncBarrier();
}

}