//===================
// DispatchedQueue.h
//===================

#pragma once


//=======
// Using
//=======

#include "DispatchedHandler.h"
#include "Mutex.h"
#include "Signal.h"


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
	static inline VOID Append(VOID (*Procedure)())
		{
		Append(new DispatchedProcedure(Procedure));
		}
	template <class _owner_t> static inline VOID Append(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		Append(new DispatchedMemberProcedure<_owner_t>(Owner, Procedure));
		}
	template <class _owner_t, class _lambda_t> static inline VOID Append(_owner_t* Owner, _lambda_t&& Lambda)
		{
		Append(new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda)));
		}
	template <class _owner_t, class _lambda_t> static inline VOID Append(Handle<_owner_t> Owner, _lambda_t&& Lambda)
		{
		Append(new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda)));
		}
	static VOID Enter();
	static VOID Exit();

private:
	// Common
	static VOID Run();
	static BOOL Wait();
	static Handle<DispatchedHandler> s_First;
	static Mutex s_Mutex;
	static Signal s_Signal;
	static BOOL s_Waiting;
};

}