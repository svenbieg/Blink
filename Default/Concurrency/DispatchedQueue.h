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
	static VOID Begin();
	static VOID Exit();
	static VOID Run();
	static BOOL Wait();

private:
	// Common
	static DispatchedHandler* s_First;
	static DispatchedHandler* s_Last;
	static Mutex s_Mutex;
	static Signal s_Signal;
	static BOOL s_Waiting;
};

}