//============
// Function.h
//============

#pragma once


//=======
// Using
//=======

#include "Handle.h"
#include <utility>


//==========
// Callable
//==========

template <class _ret_t, class... _args_t>
class Callable: public Object
{
public:
	// Con-/Destructors
	virtual ~Callable() {}

	// Common
	virtual _ret_t Call(_args_t... Arguments)=0;
};


//================
// Callable Typed
//================

template <class _lambda_t, class _ret_t, class... _args_t>
class CallableTyped: public Callable<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	CallableTyped(_lambda_t&& lambda): m_Lambda(std::move(lambda)) {}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return m_Lambda(Arguments...);
		}

private:
	// Common
	_lambda_t m_Lambda;
};


//==========
// Function
//==========

template <class _ret_t, class... _args_t>
class Function
{
public:
	// Con-/Destructors
	Function() {}
	Function(Function const& Copy): m_Callable(Copy.m_Callable) {}
	Function(Function&& Move): m_Callable(Move.m_Callable) { Move.m_Callable=nullptr; }
	template<class _lambda_t> Function(_lambda_t&& Lambda)
		{
		m_Callable=new CallableTyped<_lambda_t, _ret_t, _args_t...>(std::forward<_lambda_t>(Lambda));
		}

	// Common
	inline operator bool()const { return m_Callable!=nullptr; }
	inline _ret_t operator()(_args_t... Arguments)const
		{
		if(!m_Callable)
			throw NullPointerException();
		return m_Callable->Call(Arguments...);
		}
	inline Function& operator=(Function const& Copy)
		{
		m_Callable=Copy.m_Callable;
		return *this;
		}

private:
	// Using
	using _callable_t=Callable<_ret_t, _args_t...>;

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
