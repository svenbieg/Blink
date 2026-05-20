//===================
// DispatchedQueue.h
//===================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#main-task

#pragma once


//=======
// Using
//=======

#include "Collections/LinkedList.h"
#include "Concurrency/DispatchedHandler.h"
#include "Concurrency/CriticalSection.h"
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
	// Friends
	friend Task;

	// Con-/Destructors
	DispatchedQueue()=delete;

	// Common
	static inline Handle<DispatchedHandler> Append(VOID (*Procedure)())
		{
		auto handler=new DispatchedProcedure(Procedure);
		Append(handler);
		return handler;
		}
	template <class _owner_t> static inline Handle<DispatchedHandler> Append(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		Append(handler);
		return handler;
		}
	template <class _owner_t, class _lambda_t> static inline Handle<DispatchedHandler> Append(_owner_t* Owner, _lambda_t&& Lambda)
		{
		auto handler=new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		Append(handler);
		return handler;
		}
	template <class _owner_t, class _lambda_t> static inline Handle<DispatchedHandler> Append(Handle<_owner_t> Owner, _lambda_t&& Lambda)
		{
		auto handler=new DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		Append(handler);
		return handler;
		}
	template <class _lambda_t> static inline Handle<DispatchedHandler> Append(nullptr_t Owner, _lambda_t&& Lambda)
		{
		auto handler=new DispatchedLambda<nullptr_t, _lambda_t>(std::forward<_lambda_t>(Lambda));
		Append(handler);
		return handler;
		}
	static VOID Enter();
	static VOID Exit()noexcept;

private:
	// Common
	static VOID Append(DispatchedHandler* Handler)noexcept;
	static Handle<DispatchedHandler> s_First;
	static Handle<DispatchedHandler> s_Last;
	static CriticalSection s_CriticalSection;
	static Signal s_Signal;
	static volatile BOOL s_Waiting;
};

}