//=====================
// DispatchedQueue.cpp
//=====================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#main-task

#include "DispatchedQueue.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"


//===========
// Namespace
//===========

namespace Concurrency {


//========
// Common
//========

VOID DispatchedQueue::Enter()
{
SpinLock lock(s_CriticalSection);
s_Waiting=true;
while(s_Waiting)
	{
	while(s_First)
		{
		auto handler=s_First;
		s_First=nullptr;
		s_Last=nullptr;
		lock.Unlock();
		while(handler)
			{
			auto next=handler->m_Next;
			handler->Run();
			delete handler;
			handler=next;
			}
		lock.Lock();
		}
	s_Signal.WaitInternal(lock);
	}
}

VOID DispatchedQueue::Exit()noexcept
{
SpinLock lock(s_CriticalSection);
s_Waiting=false;
s_Signal.Trigger();
}


//================
// Common Private
//================

VOID DispatchedQueue::Append(DispatchedHandler* handler)noexcept
{
SpinLock lock(s_CriticalSection);
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

DispatchedHandler* DispatchedQueue::s_First=nullptr;
DispatchedHandler* DispatchedQueue::s_Last=nullptr;
CriticalSection DispatchedQueue::s_CriticalSection;
Signal DispatchedQueue::s_Signal;
volatile BOOL DispatchedQueue::s_Waiting=false;

}