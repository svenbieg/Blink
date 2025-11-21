//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Devices/System/System.h"
#include "Application.h"
#include "Console.h"

using namespace Blink;
using namespace Concurrency;
using namespace Devices::System;


//=============
// Entry-Point
//=============

VOID Main()
{
auto app=Application::Get();
app->Run();
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

VOID Application::Run()
{
StartBlinking();
DispatchedQueue::Enter();
}


//==========================
// Con-/Destructors Private
//==========================

Application::Application()
{}

Global<Application> Application::s_Current;

}