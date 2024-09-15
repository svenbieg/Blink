//=================
// Application.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Application.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Core {


//========
// Common
//========

Handle<Application> Application::Current;

VOID Application::HandleDispatched()
{
while(m_DispatchedHandlers)
	{
	DispatchedHandler::Remove(m_DispatchedHandlers)->Run();
	}
}

INT Application::Run()
{
ScopedLock lock(m_Mutex);
while(Running)
	{
	m_Dispatched.Wait(lock);
	HandleDispatched();
	}
return 0;
}

VOID Application::Quit()
{
Running=false;
m_DispatchedHandlers=nullptr;
}


//============================
// Con-/Destructors Protected
//============================

Application::Application(LPCSTR name, LPCSTR version):
Name(name),
Running(true),
Version(version)
{
Current=this;
}


//==================
// Common Protected
//==================

VOID Application::DispatchHandler(DispatchedHandler* handler)
{
DispatchedHandler::Append(m_DispatchedHandlers, handler);
m_Dispatched.Broadcast();
}

}
