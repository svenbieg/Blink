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
Handle<Application> app=new Application();
return app->Run();
}


//===========
// Namespace
//===========

namespace Blink {


//==================
// Con-/Destructors
//==================

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
