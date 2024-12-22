//=====================
// DispatchedQueue.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "DispatchedQueue.h"


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID DispatchedQueue::Append(DispatchedHandler* handler)
{
assert(handler->m_Next==nullptr);
TaskLock lock(s_Mutex);
if(!s_Last)
	{
	s_First=handler;
	s_Last=handler;
	return;
	}
s_Last->m_Next=handler;
s_Last=handler;
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
	s_First=nullptr;
	s_Last=nullptr;
	lock.Unlock();
	while(handler)
		{
		handler->Run();
		auto next=handler->m_Next;
		delete handler;
		handler=next;
		}
	lock.Lock();
	}
}

BOOL DispatchedQueue::Wait()
{
if(!s_Waiting)
	return false;
return s_Signal.WaitInternal();
}

DispatchedHandler* DispatchedQueue::s_First=nullptr;
DispatchedHandler* DispatchedQueue::s_Last=nullptr;
Mutex DispatchedQueue::s_Mutex;
Signal DispatchedQueue::s_Signal;
BOOL DispatchedQueue::s_Waiting=true;

}