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
if(m_BlinkTask)
	m_BlinkTask->Cancel();
m_ServiceTask->Cancel();
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
	if(!m_BlinkTask)
		m_BlinkTask=Task::Create(this, &StatusLed::BlinkTask, "blink", 1024);
	}
else
	{
	if(m_BlinkTask)
		{
		m_BlinkTask->Cancel();
		m_BlinkTask=nullptr;
		}
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

VOID StatusLed::BlinkTask()
{
auto task=Task::Get();
while(!task->Cancelled)
	{
	Task::Sleep(m_Period);
	m_On=!m_On;
	m_Signal.Trigger();
	}
}

VOID StatusLed::ServiceTask()
{
m_WifiAdapter=WifiAdapter::Create();
m_WifiAdapter->Ready.Wait(2000);
UINT mask=(1<<0);
UINT buf[2];
buf[0]=mask;
WriteLock lock(m_Mutex);
auto task=Task::Get();
while(!task->Cancelled)
	{
	buf[1]=m_On? mask: 0;
	m_WifiAdapter->SetVariable("gpioout", buf, 2*sizeof(UINT));
	m_Signal.Wait(lock);
	}
}

}}