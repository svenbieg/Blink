//============
// MainTask.h
//============

#pragma once


//=======
// Using
//=======

#include "Concurrency/DispatchedQueue.h"


//===========
// Namespace
//===========

namespace Concurrency {


//===========
// Main-Task
//===========

class MainTask
{
public:
	// Common
	static inline VOID Dispatch(VOID (*Procedure)())
		{
		auto handler=new Concurrency::DispatchedProcedure(Procedure);
		Concurrency::DispatchedQueue::Append(handler);
		}
	template <class _owner_t> static inline VOID Dispatch(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new Concurrency::DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		Concurrency::DispatchedQueue::Append(handler);
		}
	template <class _owner_t, class _lambda_t> static inline VOID Dispatch(_owner_t* Owner, _lambda_t&& Lambda)
		{
		auto handler=new Concurrency::DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		Concurrency::DispatchedQueue::Append(handler);
		}
	template <class _owner_t, class _lambda_t> static inline VOID Dispatch(Handle<_owner_t> Owner, _lambda_t&& Lambda)
		{
		auto handler=new Concurrency::DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		Concurrency::DispatchedQueue::Append(handler);
		}
};

}