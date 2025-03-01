//=================
// WifiAdapter.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/Arm/Gpio.h"
#include "WifiAdapter.h"

using namespace Concurrency;
using namespace Devices::Arm;
using namespace Devices::Emmc;


//===========
// Namespace
//===========

namespace Devices {
	namespace Wifi {


//==================
// Con-/Destructors
//==================

WifiAdapter::~WifiAdapter()
{
s_Current=nullptr;
}


//========
// Common
//========

Handle<Task> WifiAdapter::ConnectAsync()
{
return Task::Create(this, &WifiAdapter::DoConnect);
}

Handle<WifiAdapter> WifiAdapter::Get()
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
WriteLock lock(m_Mutex);
if(m_Status==WifiStatus::Reset)
	DoReset();
}

VOID WifiAdapter::DoReset()
{
Gpio::SetPinMode(28, PinMode::Output);
Gpio::DigitalWrite(28, true);
Task::Sleep(150);
Gpio::SetPinMode(30, PinMode::Func4); // SDIO_CLK
Gpio::SetPinMode(31, PinMode::Func4, PullMode::PullUp); // SDIO_CMD
Gpio::SetPinMode(32, PinMode::Func4, PullMode::PullUp); // SDIO_D0
Gpio::SetPinMode(33, PinMode::Func3, PullMode::PullUp); // SDIO_D1
Gpio::SetPinMode(34, PinMode::Func4, PullMode::PullUp); // SDIO_D2
Gpio::SetPinMode(35, PinMode::Func3, PullMode::PullUp); // SDIO_D3
m_EmmcHost=EmmcHost::Open(EmmcDevice::Wifi);
m_EmmcHost->Reset();
m_EmmcHost->Command(CMD_SEND_OP_COND, 0, nullptr);
UINT rca=m_EmmcHost->Command(CMD_SEND_REL_ADDR, 0);
m_EmmcHost->Command(CMD_SELECT_CARD, rca);
m_EmmcHost=nullptr;
}

WifiAdapter* WifiAdapter::s_Current=nullptr;

}}
