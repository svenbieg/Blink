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
	inline VOID Dispatch(VOID (*Procedure)())
		{
		DispatchedHandler* handler=new Details::DispatchedProcedure(Procedure);
		DispatchHandler(handler);
		}
	template <class _owner_t> inline VOID Dispatch(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		DispatchedHandler* handler=new Details::DispatchedMemberProcedure<_owner_t>(Owner, Procedure);
		DispatchHandler(handler);
		}
	template <class _owner_t, class _lambda_t> inline VOID Dispatch(_owner_t* Owner, _lambda_t&& Lambda)
		{
		DispatchedHandler* handler=new Details::DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		DispatchHandler(handler);
		}
	template <class _owner_t, class _lambda_t> inline VOID Dispatch(Handle<_owner_t> Owner, _lambda_t&& Lambda)
		{
		DispatchedHandler* handler=new Details::DispatchedLambda<_owner_t, _lambda_t>(Owner, std::forward<_lambda_t>(Lambda));
		DispatchHandler(handler);
		}
	virtual VOID DispatchHandler(DispatchedHandler* Handler);
	LPCSTR Name;
	virtual INT Run();
	volatile BOOL Running;
	virtual VOID Quit();
	LPCSTR Version;

protected:
	// Using
	using DispatchedHandler=Core::DispatchedHandler;

	// Con-/Destructors
	Application(LPCSTR Name, LPCSTR Version="1.0");

	// Common
	Concurrency::Signal m_Dispatched;
	Handle<DispatchedHandler> m_DispatchedHandler;
	Concurrency::Mutex m_Mutex;
};

}
