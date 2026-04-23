//============
// Function.h
//============

#pragma once


//=======
// Using
//=======

#include "Callable.h"


//==========
// Function
//==========

template <class _ret_t, class... _args_t>
class Function
{
public:
	// Con-/Destructors
	Function()=default;
	Function(Function const& Copy)noexcept: m_Callable(Copy.m_Callable) {}
	Function(Function&& Move)noexcept: m_Callable(Move.m_Callable) { Move.m_Callable=nullptr; }
	template<class _owner_t, class _lambda_t> Function(_owner_t* Owner, _lambda_t&& Lambda)
		{
		m_Callable=CallableLambda<_owner_t, _lambda_t, _ret_t, _args_t...>::Create(Owner, std::forward<_lambda_t>(Lambda));
		}

	// Common
	inline operator bool()const noexcept { return m_Callable!=nullptr; }
	inline _ret_t operator()(_args_t... Arguments)const
		{
		if(!m_Callable)
			throw NullPointerException();
		return m_Callable->Call(Arguments...);
		}
	inline Function& operator=(Function const& Copy)noexcept
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
