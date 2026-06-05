//============
// Callback.h
//============

#pragma once


//=======
// Using
//=======

#include "Callable.h"


//==========
// Callback
//==========

template <class... _args_t> class Callback
{
public:
	// Con-/Destructors
	Callback()=default;

	// Common
	VOID Call(_args_t... Arguments)
		{
		if(m_Callback)
			m_Callback->Call(Arguments...);
		}
	VOID Set(nullptr_t) { m_Callback=nullptr; }
	VOID Set(VOID (*Procedure)(_args_t...))
		{
		m_Callback=CallableFunction<VOID, _args_t...>::Create(Procedure);
		}
	template <class _owner_t> VOID Set(_owner_t* Owner, VOID (_owner_t::*Procedure)(_args_t...))
		{
		m_Callback=CallableMemberFunction<_owner_t, VOID, _args_t...>::Create(Owner, Procedure);
		}

private:
	// Common
	Handle<Callable<VOID, _args_t...>> m_Callback;
};
