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
}


//========
// Common
//========

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
m_On(false)
{
m_ServiceTask=ServiceTask::Create(this, &StatusLed::ServiceTask, "status_led", 1024);
}


//================
// Common Private
//================

VOID StatusLed::ServiceTask()
{
WriteLock lock(m_Mutex);
m_WifiAdapter=WifiAdapter::Create();
m_WifiAdapter->Ready.Wait(1000);
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