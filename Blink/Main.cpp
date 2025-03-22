//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Devices/System/System.h"

using namespace Concurrency;
using namespace Devices::System;


//=============
// Entry-Point
//=============

INT Main()
{
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
	System::Led(false);
	});
DispatchedQueue::Enter();
return 0;
}
