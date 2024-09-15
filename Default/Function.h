//============
// Function.h
//============

#pragma once


//=======
// Using
//=======

#include "Handle.h"


//==========
// Callable
//==========

namespace Details
	{
	template <class _ret_t, class... _args_t>
	class Callable: public Object
		{
		public:
			// Con-/Destructors
			virtual ~Callable() {}

			// Common
			virtual _ret_t Call(_args_t... Arguments)=0;
		};
	}


//================
// Callable Typed
//================

namespace Details {

template <class _func_t, class _ret_t, class... _args_t>
class CallableTyped: public Callable<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	CallableTyped(_func_t fn): cFunction(fn) {}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return cFunction(Arguments...);
		}

private:
	// Common
	_func_t cFunction;
};

}


//==========
// Function
//==========

template <class _ret_t, class... _args_t>
class Function
{
public:
	// Con-/Destructors
	Function() {}
	Function(Function const& Function): hCallable(Function.hCallable) {}
	template<class _func_t> Function(_func_t fn)
		{
		hCallable=new ::Details::CallableTyped<_func_t, _ret_t, _args_t...>(fn);
		}

	// Common
	inline operator bool()const { return hCallable!=nullptr; }
	inline _ret_t operator()(_args_t... Arguments)const
		{
		return hCallable->Call(Arguments...);
		}
	inline Function& operator=(nullptr_t)
		{
		hCallable=nullptr;
		return *this;
		}

private:
	// Using
	using _callable_t=::Details::Callable<_ret_t, _args_t...>;

	// Common
	Handle<_callable_t> hCallable;
};

template <class _ret_t, class... _args_t>
class Function<_ret_t(_args_t...)>: public Function<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	using _base_t=Function<_ret_t, _args_t...>;
	using _base_t::_base_t;
};

