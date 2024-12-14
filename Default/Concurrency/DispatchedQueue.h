//===================
// DispatchedQueue.h
//===================

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"
#include "DispatchedHandler.h"


//===========
// Namespace
//===========

namespace Concurrency {


//==================
// Dispatched-Queue
//==================

class DispatchedQueue
{
public:
	// Common
	static VOID Append(DispatchedHandler* Handler);
	static VOID Append(DispatchedHandler** Queue, DispatchedHandler* Handler);
	static VOID Begin();
	static VOID Exit();
	static VOID Run();

private:
	// Common
	static DispatchedHandler* s_Handler;
	static Mutex s_Mutex;
	static Signal s_Signal;
};

}