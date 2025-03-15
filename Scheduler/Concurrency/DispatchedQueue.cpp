//=====================
// DispatchedQueue.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/TaskLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID DispatchedQueue::Append(DispatchedHandler* handler)
{
TaskLock lock(s_Mutex);
DispatchedHandler::Append(&s_First, handler);
s_Signal.Trigger();
}

VOID DispatchedQueue::Enter()
{
s_Waiting=true;
while(Wait())
	Run();
}

VOID DispatchedQueue::Exit()
{
s_Waiting=false;
s_Signal.Cancel();
}


//================
// Common Private
//================

VOID DispatchedQueue::Run()
{
TaskLock lock(s_Mutex);
while(s_First)
	{
	auto handler=s_First;
	s_First=handler->m_Next;
	lock.Unlock();
	handler->Run();
	lock.Lock();
	}
}

BOOL DispatchedQueue::Wait()
{
if(!s_Waiting)
	return false;
s_Signal.WaitInternal();
return true;
}

Handle<DispatchedHandler> DispatchedQueue::s_First;
Mutex DispatchedQueue::s_Mutex;
Signal DispatchedQueue::s_Signal;
BOOL DispatchedQueue::s_Waiting=false;

}