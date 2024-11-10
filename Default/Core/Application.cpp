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


//============================
// Con-/Destructors Protected
//============================

Application::~Application()
{
Current=nullptr;
}


//========
// Common
//========

Application* Application::Current=nullptr;

VOID Application::DispatchHandler(DispatchedHandler* handler)
{
ScopedLock lock(m_Mutex);
DispatchedHandler::Append(m_DispatchedHandler, handler);
m_Dispatched.Trigger();
}

INT Application::Run()
{
ScopedLock lock(m_Mutex);
while(Running)
	{
	m_Dispatched.Wait(lock);
	while(m_DispatchedHandler)
		{
		auto handler=DispatchedHandler::Remove(m_DispatchedHandler);
		lock.Unlock();
		handler->Run();
		lock.Lock();
		}
	}
return 0;
}

VOID Application::Quit()
{
ScopedLock lock(m_Mutex);
Running=false;
m_DispatchedHandler=nullptr;
m_Dispatched.Trigger();
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

}
