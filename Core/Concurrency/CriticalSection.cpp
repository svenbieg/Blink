//=====================
// CriticalSection.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

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

VOID CriticalSection::Enter()
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
}

BOOL CriticalSection::TryEnter()
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
	return true;
	}
Interrupts::Enable();
return false;
}

VOID CriticalSection::Leave()
{
UINT core=Cpu::GetId();
if(m_Core!=core)
	return;
if(--m_LockCount>0)
	return;
Cpu::StoreAndRelease(&m_Core, CPU_COUNT);
Cpu::SetEvent();
Interrupts::Enable();
}

}
