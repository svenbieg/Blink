//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Application.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Core {


//========
// Common
//========

Application* Application::Current=nullptr;

INT Application::Run()
{
DispatchedQueue::Begin();
return 0;
}

VOID Application::Quit()
{
DispatchedQueue::Exit();
}


//============================
// Con-/Destructors Protected
//============================

Application::Application(LPCSTR name):
Name(name)
{
Current=this;
}

}