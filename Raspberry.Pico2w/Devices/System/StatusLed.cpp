//===============
// StatusLed.cpp
//===============

#include "StatusLed.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//==================
// Con-/Destructors
//==================

StatusLed::~StatusLed()
{
m_ServiceTask->Cancel();
if(m_Timer)
	m_Timer->Triggered.Remove(this);
}


//========
// Common
//========

VOID StatusLed::Blink(UINT period)
{
WriteLock lock(m_Mutex);
if(m_Period==period)
	return;
m_Period=period;
if(m_Period)
	{
	if(!m_Timer)
		m_Timer=SystemTimer::Get();
	m_Timer->Triggered.Add(this, &StatusLed::OnSystemTimer);
	}
else
	{
	m_Timer->Triggered.Remove(this);
	m_Timer=nullptr;
	}
}

VOID StatusLed::Set(BOOL on)
{
WriteLock lock(m_Mutex);
if(m_On==on)
	return;
m_On=on;
m_Signal.Trigger();
}


//==========================
// Con-/Destructors Private
//==========================

StatusLed::StatusLed():
m_On(false),
m_Period(0)
{
m_ServiceTask=ServiceTask::Create(this, &StatusLed::ServiceTask, "status_led", 1024);
}


//================
// Common Private
//================

VOID StatusLed::OnSystemTimer()
{
m_Ticks+=10;
if(m_Ticks>=m_Period)
	{
	m_Ticks=0;
	m_On=!m_On;
	m_Signal.Trigger();
	}
}

VOID StatusLed::ServiceTask()
{
WriteLock lock(m_Mutex);
m_WifiAdapter=WifiAdapter::Create();
m_WifiAdapter->Ready.Wait(2000);
UINT mask=(1<<0);
UINT buf[2];
buf[0]=mask;
auto task=Task::Get();
while(!task->Cancelled)
	{
	buf[1]=m_On? mask: 0;
	m_WifiAdapter->SetVariable("gpioout", buf, 2*sizeof(UINT));
	m_Signal.Wait(lock);
	}
}

}}