//=================
// WifiAdapter.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/Arm/Gpio.h"
#include "Devices/System/Peripherals.h"
#include "WifiAdapter.h"

using namespace Concurrency;
using namespace Devices::Arm;
using namespace Devices::System;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//========
// Common
//========

Handle<Task> WifiAdapter::ConnectAsync()
{
auto task=CreateTask(this, &WifiAdapter::DoConnect);
task->Then([this]()
	{
	if(m_Status==WifiStatus::Online)
		{
		Connected(this);
		}
	else
		{
		ConnectionFailed(this);
		}
	});
return task;
}

Handle<WifiAdapter> WifiAdapter::Open()
{
if(!s_Current)
	s_Current=new WifiAdapter();
return s_Current;
}


//==========================
// Con-/Destructors Private
//==========================

WifiAdapter::WifiAdapter():
m_Status(WifiStatus::Reset)
{}


//================
// Common Private
//================

VOID WifiAdapter::DoConnect()
{
ScopedLock lock(m_Mutex);
if(m_Status==WifiStatus::Reset)
	DoInitialize();
}

VOID WifiAdapter::DoInitialize()
{
Gpio::SetPinMode(28, PinMode::Output);
Gpio::DigitalWrite(28, true);
Sleep(150);
Gpio::SetPinMode(30, PinMode::Func4);
Gpio::SetPinMode(31, PinMode::Func4, PullMode::PullUp);
Gpio::SetPinMode(32, PinMode::Func4, PullMode::PullUp);
Gpio::SetPinMode(33, PinMode::Func3, PullMode::PullUp);
Gpio::SetPinMode(34, PinMode::Func4, PullMode::PullUp);
Gpio::SetPinMode(35, PinMode::Func3, PullMode::PullUp);
m_EmmcDevice=new EmmcDevice(AXI_EMMC_BASE, IRQ_SDIO2);
m_EmmcDevice->Initialize();
}

Handle<WifiAdapter> WifiAdapter::s_Current;

}}
