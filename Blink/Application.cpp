//=================
// Application.cpp
//=================

#include "pch.h"


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
{
InitializeCommands();
}

Global<Application> Application::s_Current;


//================
// Common Private
//================

VOID Application::InitializeCommands()
{
m_Commands.add("off", []()
	{
	Console::Print("Led off\n");
	System::Led(false);
	});
m_Commands.add("on", []()
	{
	Console::Print("Led on\n");
	System::Led(true);
	});
m_Commands.add("start", []()
	{
	auto app=Application::Get();
	app->StartBlinking();
	});
m_Commands.add("stop", []()
	{
	auto app=Application::Get();
	app->StopBlinking();
	});
auto console=Console::Get();
console->CommandReceived.Add(this, &Application::OnConsoleCommandReceived);
console->Print("Blink commands:");
for(auto cmd: m_Commands)
	{
	auto name=cmd.get_key();
	console->Print(" ");
	console->Print(name);
	}
console->Print("\n");
}

VOID Application::OnConsoleCommandReceived(Handle<String> cmd)
{
auto func=m_Commands.get(cmd);
if(func)
	func();
}

}