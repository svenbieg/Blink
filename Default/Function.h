//============
// Function.h
//============

#pragma once


//=======
// Using
//=======

#include <utility>
#include "Handle.h"
#include "Object.h"


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

template <class _lambda_t, class _ret_t, class... _args_t>
class CallableTyped: public Callable<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	CallableTyped(_lambda_t&& fn): m_Lambda(std::move(fn)) {}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return m_Lambda(Arguments...);
		}

private:
	// Common
	_lambda_t m_Lambda;
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
	Function(Function const& Function): m_Callable(Function.m_Callable) {}
	template<class _lambda_t> Function(_lambda_t&& Lambda)
		{
		m_Callable=new ::Details::CallableTyped<_lambda_t, _ret_t, _args_t...>(std::forward<_lambda_t>(Lambda));
		}

	// Common
	inline operator bool()const { return m_Callable!=nullptr; }
	inline _ret_t operator()(_args_t... Arguments)const
		{
		return m_Callable->Call(Arguments...);
		}
	inline Function& operator=(nullptr_t)
		{
		m_Callable=nullptr;
		return *this;
		}

private:
	// Using
	using _callable_t=::Details::Callable<_ret_t, _args_t...>;

	// Common
	Handle<_callable_t> m_Callable;
};

template <class _ret_t, class... _args_t>
class Function<_ret_t(_args_t...)>: public Function<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	using _base_t=Function<_ret_t, _args_t...>;
	using _base_t::_base_t;
};

