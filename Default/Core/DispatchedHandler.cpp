//=======================
// DispatchedHandler.cpp
//=======================

#include "pch.h"


//=======
// Using
//=======

#include "DispatchedHandler.h"


//===========
// Namespace
//===========

namespace Core {


//========
// Common
//========

VOID DispatchedHandler::Append(Handle<DispatchedHandler>& dispatched, DispatchedHandler* handler)
{
if(!dispatched)
	{
	dispatched=handler;
	return;
	}
auto current=dispatched;
while(current->m_Next)
	current=current->m_Next;
current->m_Next=handler;
}

VOID DispatchedHandler::Remove(Handle<DispatchedHandler>& dispatched, DispatchedHandler* handler)
{
if(dispatched==handler)
	{
	dispatched=handler->m_Next;
	return;
	}
auto current=dispatched;
while(current->m_Next)
	{
	auto next=current->m_Next;
	if(next==handler)
		{
		current->m_Next=next->m_Next;
		return;
		}
	current=next;
	}
}

}
