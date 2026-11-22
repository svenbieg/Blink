//===============
// StatusLed.cpp
//===============

#include "StatusLed.h"


//=======
// Using
//=======

#include "Concurrency/ServiceTask.h"

using namespace Concurrency;
using namespace Devices::Wifi;


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
}


//========
// Common
//========

VOID StatusLed::Blink(UINT period)
{
WriteLock lock(m_Mutex);
m_Period=period;
if(!m_BlinkTask)
	m_BlinkTask=Task::Create(this, &StatusLed::BlinkTask, "blink", 1024);
}

VOID StatusLed::Set(BOOL on)
{
WriteLock lock(m_Mutex);
m_On=on;
m_Signal.Trigger();
}

VOID StatusLed::Stop()
{
WriteLock lock(m_Mutex);
if(m_BlinkTask)
	{
	m_BlinkTask->Cancel();
	m_BlinkTask=nullptr;
	}
m_Period=0;
}

VOID StatusLed::Switch()
{
WriteLock lock(m_Mutex);
m_On=!m_On;
m_Signal.Trigger();
}


//==========================
// Con-/Destructors Private
//==========================

StatusLed::StatusLed():
m_On(false),
m_Period(0)
{
m_Wifi=WifiAdapter::Create();
m_ServiceTask=ServiceTask::Create(this, &StatusLed::ServiceTask, "status_led");
m_Mutex.SetOwner(m_ServiceTask);
m_ServiceTask->Run();
}


//================
// Common Private
//================

VOID StatusLed::BlinkTask()
{
while(1)
	{
	Task::Sleep(m_Period);
	Switch();
	}
}

VOID StatusLed::ServiceTask()
{
WriteLock lock(m_Mutex, WriteLockMode::Owner);
constexpr UINT BIT_MASK=(1<<0);
UINT buf[2];
buf[0]=BIT_MASK;
while(1)
	{
	buf[1]=m_On? BIT_MASK: 0;
	m_Wifi->SetVariable("gpioout", buf, 2*sizeof(UINT));
	m_Signal.Wait(lock);
	}
}

}}