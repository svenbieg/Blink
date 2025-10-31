//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/System/System.h"
#include "UI/Console.h"

using namespace Concurrency;
using namespace Devices::System;
using namespace UI;


//=============
// Entry-Point
//=============

VOID Main()
{
Console::Print("Starting to blink...\n");
auto task=Task::Create(nullptr, []()
	{
	BOOL led=false;
	for(UINT loops=0; loops<5; loops++)
		{
		System::Led(false);
		Task::Sleep(500);
		System::Led(true);
		Task::Sleep(500);
		}
	}, "blink");
task->Then(nullptr, []()
	{
	Console::Print("Done\n");
	System::Led(false);
	});
DispatchedQueue::Enter();
}
