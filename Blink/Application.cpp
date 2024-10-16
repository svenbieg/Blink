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


//=============
// Entry-Point
//=============

INT Main()
{
auto app=new Blink::Application();
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
Core::Application("Blink")
{
auto timer=SystemTimer::Open();
timer->Tick.Add(this, &Application::OnSystemTimerTick);
}


//================
// Common Private
//================

BOOL led=false;
UINT loops=0;

VOID Application::OnSystemTimerTick()
{
if(++loops<50)
	return;
led=!led;
auto system=System::Open();
system->Led(led);
loops=0;
}

}
