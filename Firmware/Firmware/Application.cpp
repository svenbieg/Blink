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

namespace Firmware {


//========
// Common
//========

Application* Application::Current=nullptr;

INT Application::Run()
{
DispatchedQueue::Enter();
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
m_Name(name)
{
Current=this;
}

}