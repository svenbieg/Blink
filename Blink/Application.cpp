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
m_BlinkingTask=Task::Create(nullptr, []()
	{
	auto task=Task::Get();
	while(!task->Cancelled)
		{
		System::Led(false);
		Task::Sleep(500);
		System::Led(true);
		Task::Sleep(500);
		}
	}, "blink");
m_BlinkingTask->Then(this, [this]()
	{
	Console::Print("Stopped blinking\n");
	System::Led(false);
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
auto console=Console::Get();
console->AddCommand("off", []()
	{
	Console::Print("Led off\n");
	System::Led(false);
	});
console->AddCommand("on", []()
	{
	Console::Print("Led on\n");
	System::Led(true);
	});
console->AddCommand("restart", []()
	{
	Console::Print("Restarting...\n");
	System::Restart();
	});
console->AddCommand("start", []()
	{
	auto app=Application::Get();
	app->StartBlinking();
	});
console->AddCommand("stop", []()
	{
	auto app=Application::Get();
	app->StopBlinking();
	});
}

Application* Application::s_Current=nullptr;

}