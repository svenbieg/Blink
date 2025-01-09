//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "Devices/Timers/SystemTimer.h"
#include "Application.h"

using namespace Devices::System;
using namespace Devices::Timers;

using Application=Blink::Application;


//=============
// Entry-Point
//=============

INT Main()
{
auto app=Application::Create();
app->Run();
return 0;
}


//===========
// Namespace
//===========

namespace Blink {


//==========================
// Con-/Destructors Private
//==========================

Application::Application():
Firmware::Application("Blink")
{
auto timer=SystemTimer::Get();
timer->Triggered.Add(this, &Application::OnSystemTimerTick);
}


//================
// Common Private
//================

BOOL led=false;
UINT loops=0;

VOID Application::OnSystemTimerTick()
{
if(++loops%50)
	return;
led=!led;
System::Led(led);
}

}