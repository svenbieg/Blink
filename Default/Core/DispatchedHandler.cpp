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
#ifdef _DEBUG
UINT count=0;
while(current->hNext)
	{
	current=current->hNext;
	count++;
	}
assert(count<10);
#else
while(current->hNext)
	{
	current=current->hNext;
	}
#endif
current->hNext=handler;
}

VOID DispatchedHandler::Remove(Handle<DispatchedHandler>& dispatched, DispatchedHandler* handler)
{
if(dispatched==handler)
	{
	dispatched=handler->hNext;
	return;
	}
auto current=dispatched;
while(current->hNext)
	{
	auto next=current->hNext;
	if(next==handler)
		{
		current->hNext=next->hNext;
		return;
		}
	current=next;
	}
}

}
