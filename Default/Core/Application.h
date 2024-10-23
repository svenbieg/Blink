//===============
// Application.h
//===============

#pragma once


//=======
// Using
//=======

#include "DispatchedHandler.h"


//===========
// Namespace
//===========

namespace Core {


//=============
// Application
//=============

class Application: public Object
{
public:
	// Common
	static Handle<Application> Current;
	virtual VOID Dispatch(DispatchedHandler* Handler);
	template <class... _args_t> inline VOID Dispatch(VOID (*Procedure)(_args_t...), _args_t... Arguments)
		{
		DispatchedHandler* handler=new Details::DispatchedProcedure<_args_t...>(Procedure, Arguments...);
		Dispatch(handler);
		}
	template <class _owner_t, class... _args_t> inline VOID Dispatch(_owner_t* Owner, VOID (_owner_t::*Procedure)(_args_t...), _args_t... Arguments)
		{
		DispatchedHandler* handler=new Details::DispatchedMemberFunction<_owner_t, _args_t...>(Owner, Procedure, Arguments...);
		Dispatch(handler);
		}
	template <class _owner_t, class... _args_t> inline VOID Dispatch(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_args_t...), _args_t... Arguments)
		{
		DispatchedHandler* handler=new Details::DispatchedMemberFunction<_owner_t, _args_t...>(Owner, Procedure, Arguments...);
		Dispatch(handler);
		}
	template <class _owner_t, class _lambda_t, class... _args_t> inline VOID Dispatch(_owner_t* Owner, _lambda_t Lambda, _args_t... Arguments)
		{
		DispatchedHandler* handler=new Details::DispatchedFunction<_owner_t, _lambda_t, _args_t...>(Owner, Lambda, Arguments...);
		Dispatch(handler);
		}
	VOID HandleDispatched();
	LPCSTR Name;
	virtual INT Run();
	volatile BOOL Running;
	VOID Quit();
	LPCSTR Version;

protected:
	// Using
	using DispatchedHandler=Core::DispatchedHandler;

	// Con-/Destructors
	Application(LPCSTR Name, LPCSTR Version="1.0");

	// Common
	Concurrency::Signal m_Dispatched;
	Handle<DispatchedHandler> m_DispatchedHandlers;
	Concurrency::Mutex m_Mutex;
};

}
