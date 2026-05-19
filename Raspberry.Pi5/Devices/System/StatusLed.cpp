//===============
// StatusLed.cpp
//===============

#include "StatusLed.h"


//=======
// Using
//=======

#include "Devices/Gpio/GpioHelper.h"

using namespace Devices::Gpio;


//===========
// Namespace
//===========

namespace Devices {
	namespace System {


//========
// Common
//========

VOID StatusLed::Blink(UINT period)
{
if(m_Period==period)
	return;
m_Period=period;
if(m_Period)
	{
	if(!m_Task)
		m_Task=Task::Create(this, &StatusLed::BlinkTask);
	}
else
	{
	if(m_Task)
		{
		m_Task->Cancel();
		m_Task=nullptr;
		}
	}
}

VOID StatusLed::Set(BOOL on)
{
if(m_On==on)
	return;
m_On=on;
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, on);
}


//==========================
// Con-/Destructors Private
//==========================

StatusLed::StatusLed():
m_On(false),
m_Period(0)
{
GpioHelper::SetPinMode(GpioArmPin::ActivityLed, GpioArmPinMode::Output);
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, false);
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
	Set(!m_On);
	}
}

}}