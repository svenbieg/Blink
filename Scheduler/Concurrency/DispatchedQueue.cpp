//=====================
// DispatchedQueue.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"
#include "Concurrency/WriteLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID DispatchedQueue::Append(DispatchedHandler* handler)
{
WriteLock lock(s_Mutex);
if(!s_First)
	{
	s_First=handler;
	s_Last=handler;
	}
else
	{
	s_Last->m_Next=handler;
	s_Last=handler;
	}
s_Signal.Trigger();
}

VOID DispatchedQueue::Enter()
{
WriteLock lock(s_Mutex);
s_Waiting=true;
while(s_Waiting)
	{
	s_Signal.WaitInternal(lock);
	while(s_First)
		{
		auto handler=s_First;
		s_First=handler->m_Next;
		if(!s_First)
			s_Last=nullptr;
		lock.Unlock();
		handler->Run();
		delete handler;
		lock.Lock();
		}
	}
}

VOID DispatchedQueue::Exit()
{
WriteLock lock(s_Mutex);
s_Waiting=false;
s_Signal.Trigger();
}


//================
// Common Private
//================

DispatchedHandler* DispatchedQueue::s_First=nullptr;
DispatchedHandler* DispatchedQueue::s_Last=nullptr;
CriticalMutex DispatchedQueue::s_Mutex;
Signal DispatchedQueue::s_Signal;
volatile BOOL DispatchedQueue::s_Waiting=false;

}