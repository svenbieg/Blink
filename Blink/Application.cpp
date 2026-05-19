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
auto app=Application::Create();
DispatchedQueue::Enter();
}


//===========
// Namespace
//===========

namespace Blink {


//==========================
// Con-/Destructors Private
//==========================

Application::Application()
{
m_StatusLed=StatusLed::Create();
auto console=Console::Create();
console->AddCommand("off", this, [this]()
	{
	Console::Print("Led off\n");
	m_StatusLed->Set(false);
	});
console->AddCommand("on", this, [this]()
	{
	Console::Print("Led on\n");
	m_StatusLed->Set(true);
	});
console->AddCommand("restart", nullptr, []()
	{
	Console::Print("Restarting...\n");
	System::Restart();
	});
console->AddCommand("start", this, [this]()
	{
	if(m_StatusLed->IsBlinking())
		{
		Console::Print("Already blinking\n");
		return;
		}
	Console::Print("Starting to blink...\n");
	m_StatusLed->Blink(500);
	});
console->AddCommand("stop", this, [this]()
	{
	if(!m_StatusLed->IsBlinking())
		{
		Console::Print("Not blinking\n");
		return;
		}
	Console::Print("Stop\n");
	m_StatusLed->Stop();
	});
console->Command("start");
}

}