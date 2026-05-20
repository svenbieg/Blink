//=====================
// DispatchedHandler.h
//=====================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Scheduler/wiki#main-task

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"
#include "Handle.h"
#include <utility>


//===========
// Namespace
//===========

namespace Concurrency {


//======================
// Forward-Declarations
//======================

class DispatchedQueue;


//====================
// Dispatched-Handler
//====================

class DispatchedHandler: public Object
{
public:
	// Friends
	friend DispatchedQueue;

	// Con-/Destructors
	virtual ~DispatchedHandler() {}

	// Common
	virtual VOID Run()=0;
	inline VOID Wait() { m_Signal.Wait(); }

protected:
	// Con-/Destructors
	DispatchedHandler()=default;

	// Common
	Handle<DispatchedHandler> m_Next;
	Signal m_Signal;
};


//===========
// Procedure
//===========

class DispatchedProcedure: public DispatchedHandler
{
public:
	// Friends
	friend DispatchedQueue;
	friend Task;

	// Using
	typedef VOID (*proc_t)();

	// Common
	inline VOID Run()override
		{
		(*m_Procedure)();
		m_Signal.Trigger();
		}

private:
	// Con-/Destructors
	DispatchedProcedure(proc_t Procedure)noexcept: m_Procedure(Procedure) {}

	// Common
	proc_t m_Procedure;
};


//==================
// Member-Procedure
//==================

template <class _owner_t>
class DispatchedMemberProcedure: public DispatchedHandler
{
public:
	// Friends
	friend DispatchedQueue;
	friend Task;

	// Procedure
	typedef VOID (_owner_t::*_proc_t)();

	// Common
	inline VOID Run()override
		{
		(m_Owner->*m_Procedure)();
		m_Signal.Trigger();
		}

private:
	// Con-/Destructors
	DispatchedMemberProcedure(_owner_t* Owner, _proc_t Procedure)noexcept:
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

	// Common
	_proc_t m_Procedure;
	Handle<_owner_t> m_Owner;
};


//========
// Lambda
//========

template <class _owner_t, class _lambda_t>
class DispatchedLambda: public DispatchedHandler
{
public:
	// Friends
	friend DispatchedQueue;
	friend Task;

	// Common
	inline VOID Run()override
		{
		m_Lambda();
		m_Signal.Trigger();
		}

private:
	// Con-/Destructors
	DispatchedLambda(_owner_t* Owner, _lambda_t&& Lambda)noexcept:
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner) {}

	// Common
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};

template <class _lambda_t>
class DispatchedLambda<nullptr_t, _lambda_t>: public DispatchedHandler
{
public:
	// Friends
	friend DispatchedQueue;
	friend Task;

	// Common
	inline VOID Run()override
		{
		m_Lambda();
		m_Signal.Trigger();
		}

private:
	// Con-/Destructors
	DispatchedLambda(_lambda_t&& Lambda)noexcept:
		m_Lambda(std::move(Lambda)) {}

	// Common
	_lambda_t m_Lambda;
};

}