//=====================
// DispatchedQueue.cpp
//=====================

#include "pch.h"


//=======
// Using
//=======

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
ScopedLock lock(s_Mutex);
Append(&s_Handler, handler);
s_Signal.Trigger();
}

VOID DispatchedQueue::Append(DispatchedHandler** queue, DispatchedHandler* handler)
{
auto current_ptr=queue;
while(*current_ptr)
	{
	auto current=*current_ptr;
	current_ptr=&current->m_Next;
	}
*current_ptr=handler;
}

VOID DispatchedQueue::Begin()
{
ScopedLock lock(s_Mutex);
while(s_Signal.Wait(lock))
	Run();
}

VOID DispatchedQueue::Exit()
{
s_Signal.Cancel();
}

VOID DispatchedQueue::Run()
{
ScopedLock lock(s_Mutex);
while(s_Handler)
	{
	auto handler=s_Handler;
	s_Handler=handler->m_Next;
	lock.Unlock();
	handler->Run();
	delete handler;
	lock.Lock();
	}
}


//================
// Common Private
//================

DispatchedHandler* DispatchedQueue::s_Handler;
Mutex DispatchedQueue::s_Mutex;
Signal DispatchedQueue::s_Signal;

}