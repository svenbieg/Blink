//=====================
// DispatchedHandler.h
//=====================

#pragma once


//=======
// Using
//=======

#include "Function.h"


//===========
// Namespace
//===========

namespace Core {


//====================
// Dispatched-Handler
//====================

class DispatchedHandler: public Object
{
public:
	// Con-/Destructors
	virtual ~DispatchedHandler() {}

	// Common
	static VOID Append(Handle<DispatchedHandler>& Dispatched, DispatchedHandler* Handler);
	inline static Handle<DispatchedHandler> Remove(Handle<DispatchedHandler>& Dispatched)
		{
		Handle<DispatchedHandler> dispatched=Dispatched;
		if(dispatched)
			Dispatched=dispatched->hNext;
		return dispatched;
		}
	static VOID Remove(Handle<DispatchedHandler>& Dispatched, DispatchedHandler* Handler);
	virtual VOID Run()=0;

protected:
	// Con-/Destructors
	DispatchedHandler() {}

	// Common
	Handle<DispatchedHandler> hNext;
};


//===========
// Procedure
//===========

namespace Details {

class DispatchedProcedure: public DispatchedHandler
{
public:
	// Using
	typedef VOID (*_proc_t)();

	// Con-/Destructors
	DispatchedProcedure(_proc_t Procedure): pProcedure(Procedure) {}

	// Common
	inline VOID Run()override { (*pProcedure)(); }

private:
	// Common
	_proc_t pProcedure;
};}


//==========================
// Procedure with Arguments
//==========================

namespace Details {

template <class... _args_t>
class DispatchedProcedureWithArgs: public DispatchedHandler
{
public:
	// Using
	typedef VOID (*_proc_t)(_args_t...);

	// Con-/Destructors
	DispatchedProcedureWithArgs(_proc_t Procedure, _args_t... Arguments):
		cFunction([Procedure, Arguments...](){ (*Procedure)(Arguments...); })
		{}

	// Common
	inline VOID Run()override { cFunction(); }

private:
	// Common
	Function<VOID()> cFunction;
};}


//==========
// Function
//==========

namespace Details {

template <class _owner_t, class... _args_t>
class DispatchedFunction: public DispatchedHandler
{
public:
	// Con-/Destructors
	DispatchedFunction(_owner_t* Owner, Function<VOID(_args_t...)> Function, _args_t... Arguments):
		cFunction(Function),
		hOwner(Owner) {}

	// Common
	inline VOID Run()override { cFunction(); }

private:
	// Common
	Function<VOID(_args_t...)> cFunction;
	Handle<_owner_t> hOwner;
};}


//=================
// Member-Function
//=================

namespace Details {

template <class _owner_t, class... _args_t>
class DispatchedMemberFunction: public DispatchedHandler
{
public:
	// Procedure
	typedef VOID (_owner_t::*_proc_t)(_args_t...);

	// Con-/Destructors
	DispatchedMemberFunction(_owner_t* Owner, _proc_t Procedure, _args_t... Arguments):
		cFunction([Owner, Procedure, Arguments...]() { (Owner->*Procedure)(Arguments...); }),
		hOwner(Owner) {}

	// Common
	inline VOID Run()override { cFunction(); }

private:
	// Common
	Function<VOID()> cFunction;
	Handle<_owner_t> hOwner;
};}

}
