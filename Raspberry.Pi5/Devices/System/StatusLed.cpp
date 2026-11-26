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
WriteLock lock(m_Mutex);
m_Period=period;
if(!m_BlinkTask)
	m_BlinkTask=Task::Create(this, &StatusLed::BlinkTask, "blink");
}

VOID StatusLed::Set(BOOL on)
{
WriteLock lock(m_Mutex);
m_On=on;
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, m_On);
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
GpioHelper::DigitalWrite(GpioArmPin::ActivityLed, m_On);
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
while(1)
	{
	Task::Sleep(m_Period);
	Switch();
	}
}

}}