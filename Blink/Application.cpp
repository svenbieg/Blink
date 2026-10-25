//=================
// Application.cpp
//=================

#include "Application.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "UI/Console.h"

using namespace Blink;
using namespace Concurrency;
using namespace Devices::System;
using namespace UI;


//=============
// Entry-Point
//=============

VOID Main()
{
Task::Create(nullptr, [](){
	auto app=Application::Create();
	app->Run();
	}, "app");
DispatchedQueue::Enter();
}


//===========
// Namespace
//===========

namespace Blink {


//========
// Common
//========

VOID Application::Blink()
{
if(m_StatusLed->IsBlinking())
	{
	m_Console->Print("Already blinking\n");
	return;
	}
m_Console->Print("Starting to blink...\n");
m_StatusLed->Blink(500);
}

VOID Application::Off()
{
m_Console->Print("Led off\n");
m_StatusLed->Set(false);
}

VOID Application::On()
{
m_Console->Print("Led on\n");
m_StatusLed->Set(true);
}

VOID Application::Run()
{
Blink();
}

VOID Application::Stop()
{
if(!m_StatusLed->IsBlinking())
	{
	m_Console->Print("Not blinking\n");
	return;
	}
m_Console->Print("Stop\n");
m_StatusLed->Stop();
}


//==========================
// Con-/Destructors Private
//==========================

Application::Application()
{
m_Console=Console::Create();
m_StatusLed=StatusLed::Create();
m_Console->AddCommand("blink", this, &Application::Blink);
m_Console->AddCommand("off", this, &Application::Off);
m_Console->AddCommand("on", this, &Application::On);
m_Console->AddCommand("stop", this, &Application::Stop);
}

}