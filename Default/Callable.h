//============
// Callable.h
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

protected:
	// Con-/Destructors
	Callable()=default;
};


//==========
// Function
//==========

template <class _ret_t, class... _args_t>
class CallableFunction: public Callable<_ret_t, _args_t...>
{
public:
	// Using
	typedef _ret_t (*_func_t)(_args_t...);

	// Con-/Destructors
	static Handle<CallableFunction> Create(_func_t Function)
		{
		return new CallableFunction(Function);
		}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return (*m_Function)(Arguments...);
		}

private:
	// Con-/Destructors
	CallableFunction(_func_t Function)noexcept:
		m_Function(Function)
		{}

	// Common
	_func_t m_Function;
};


//=================
// Member-Function
//=================

template <class _owner_t, class _ret_t, class... _args_t>
class CallableMemberFunction: public Callable<_ret_t, _args_t...>
{
public:
	// Using
	typedef _ret_t (_owner_t::*_func_t)(_args_t...);

	// Con-/Destructors
	static Handle<CallableMemberFunction> Create(_owner_t* Owner, _func_t Function)
		{
		return new CallableMemberFunction(Owner, Function);
		}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return (m_Owner->*m_Function)(Arguments...);
		}

private:
	// Con-/Destructors
	CallableMemberFunction(_owner_t* Owner, _func_t Function)noexcept:
		m_Function(Function),
		m_Owner(Owner)
		{}

	// Common
	_func_t m_Function;
	Handle<_owner_t> m_Owner;
};


//========
// Lambda
//========

template <class _owner_t, class _lambda_t, class _ret_t, class... _args_t>
class CallableLambda: public Callable<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	static Handle<CallableLambda> Create(_owner_t* Owner, _lambda_t&& Lambda)
		{
		return new CallableLambda(Owner, std::forward<_lambda_t>(Lambda));
		}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return m_Lambda(Arguments...);
		}

private:
	// Con-/Destructors
	CallableLambda(_owner_t* Owner, _lambda_t&& Lambda)noexcept:
		m_Lambda(std::move(Lambda)),
		m_Owner(Owner)
		{}

	// Common
	_lambda_t m_Lambda;
	Handle<_owner_t> m_Owner;
};

template <class _lambda_t, class _ret_t, class... _args_t>
class CallableLambda<nullptr_t, _lambda_t, _ret_t, _args_t...>: public Callable<_ret_t, _args_t...>
{
public:
	// Con-/Destructors
	static Handle<CallableLambda> Create(_lambda_t&& Lambda)
		{
		return new CallableLambda(std::forward<_lambda_t>(Lambda));
		}

	// Common
	_ret_t Call(_args_t... Arguments)override
		{
		return m_Lambda(Arguments...);
		}

private:
	// Con-/Destructors
	CallableLambda(_lambda_t&& Lambda)noexcept:
		m_Lambda(std::move(Lambda))
		{}

	// Common
	_lambda_t m_Lambda;
};
