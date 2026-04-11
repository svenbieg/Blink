//=================
// Application.cpp
//=================

#include "Platform.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "UI/Console.h"
#include "Application.h"

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
app->StartBlinking();
DispatchedQueue::Enter();
}


//===========
// Namespace
//===========

namespace Blink {


//========
// Common
//========

VOID Application::StartBlinking()
{
if(m_BlinkingTask)
	{
	Console::Print("Already blinking\n");
	return;
	}
Console::Print("Starting to blink...\n");
m_BlinkingTask=Task::Create(this, [this]()
	{
	auto task=Task::Get();
	while(!task->Cancelled)
		{
		m_StatusLed->Set(false);
		Task::Sleep(500);
		m_StatusLed->Set(true);
		Task::Sleep(500);
		}
	}, "blink");
m_BlinkingTask->Then(this, [this]()
	{
	Console::Print("Stopped blinking\n");
	m_StatusLed->Set(false);
	m_BlinkingTask=nullptr;
	});
}

VOID Application::StopBlinking()
{
if(!m_BlinkingTask)
	{
	Console::Print("Not blinking\n");
	return;
	}
m_BlinkingTask->Cancel();
}


//==========================
// Con-/Destructors Private
//==========================

Application::Application()
{
m_StatusLed=StatusLed::Create();
auto console=Console::Get();
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
console->AddCommand("start", this, &Application::StartBlinking);
console->AddCommand("stop", this, &Application::StopBlinking);
}

}