//===================
// DispatchedQueue.h
//===================

// Copyright 2025, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#main-task

#pragma once


//=======
// Using
//=======

#include "Concurrency/DispatchedHandler.h"
#include "Concurrency/CriticalMutex.h"
#include "Concurrency/Signal.h"


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
	template <class _lambda_t> static inline VOID Append(nullptr_t Owner, _lambda_t&& Lambda)
		{
		Append(new DispatchedLambda<nullptr_t, _lambda_t>(std::forward<_lambda_t>(Lambda)));
		}
	static VOID Enter();
	static VOID Exit();

private:
	// Common
	static DispatchedHandler* s_First;
	static DispatchedHandler* s_Last;
	static CriticalMutex s_Mutex;
	static Signal s_Signal;
	static volatile BOOL s_Waiting;
};

}