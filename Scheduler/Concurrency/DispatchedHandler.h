//=====================
// DispatchedHandler.h
//=====================

#pragma once


//=======
// Using
//=======

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

class DispatchedHandler
{
public:
	// Friends
	friend DispatchedQueue;

	// Con-/Destructors
	virtual ~DispatchedHandler() {}

	// Common
	virtual VOID Run()=0;

protected:
	// Con-/Destructors
	DispatchedHandler(): m_Next(nullptr) {}

	// Common
	DispatchedHandler* m_Next;
};


//===========
// Procedure
//===========

class DispatchedProcedure: public DispatchedHandler
{
public:
	// Using
	typedef VOID (*proc_t)();

	// Con-/Destructors
	DispatchedProcedure(proc_t Procedure): m_Procedure(Procedure) {}

	// Common
	inline VOID Run()override { (*m_Procedure)(); }

private:
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
	// Procedure
	typedef VOID (_owner_t::*_proc_t)();

	// Con-/Destructors
	DispatchedMemberProcedure(_owner_t* Owner, _proc_t Procedure):
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

	// Common
	inline VOID Run()override { (m_Owner->*m_Procedure)(); }

private:
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
	// Con-/Destructors
	DispatchedLambda(_owner_t* Owner, _lambda_t&& Lambda):
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner) {}

	// Common
	inline VOID Run()override { m_Lambda(); }

private:
	// Common
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};

}