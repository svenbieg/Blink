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


//==================
// Con-/Destructors
//==================

Application::~Application()
{
s_Current=nullptr;
}


//========
// Common
//========

VOID Application::Run()
{
DispatchedQueue::Enter();
}

VOID Application::Quit()
{
DispatchedQueue::Exit();
}


//============================
// Con-/Destructors Protected
//============================

Application::Application(LPCTSTR name):
m_Name(name)
{
s_Current=this;
}


//================
// Common Private
//================

Application* Application::s_Current=nullptr;

}