//================
// EventHandler.h
//================

#pragma once


//=======
// Using
//=======

#include "Function.h"


//===========
// Namespace
//===========

namespace Details {


//======================
// Forward-Declarations
//======================

template <class _sender_t, class... _args_t> class EventBase;


//===============
// Event-Handler
//===============

template <class _sender_t, class... _args_t>
class EventHandler: public Object
{
public:
	// Friends
	friend class EventBase<_sender_t, _args_t...>;

	// Con-/Destructors
	virtual ~EventHandler() {}

	// Common
	virtual VOID* GetOwner()const=0;
	virtual VOID Invalidate()=0;
	virtual VOID Run(_sender_t* Sender, _args_t... Arguments)=0;

protected:
	// Con-/Destructors
	EventHandler() {}

	// Common
	Handle<EventHandler> hNext;
};


//===========
// Procedure
//===========

template <class _sender_t, class... _args_t>
class EventProcedure: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (*_func_t)();

	// Con-/Destructors
	EventProcedure(_func_t Procedure): pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return (VOID*)pProcedure; }
	inline VOID Invalidate()override { pProcedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(pProcedure)
			(*pProcedure)();
		}

private:
	// Common
	_func_t pProcedure;
};


//==========================
// Procedure with Arguments
//==========================

template <class _sender_t, class... _args_t>
class EventProcedureWithArgs: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (*_func_t)(_args_t...);

	// Con-/Destructors
	EventProcedureWithArgs(_func_t Procedure): pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return pProcedure; }
	inline VOID Invalidate()override { pProcedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(pProcedure)
			(*pProcedure)(Arguments...);
		}

private:
	// Common
	_func_t pProcedure;
};


//=======================
// Procedure with Sender
//=======================

template <class _sender_t, class... _args_t>
class EventProcedureWithSender: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (*_func_t)(_sender_t*, _args_t...);

	// Con-/Destructors
	EventProcedureWithSender(_func_t Procedure): pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return pProcedure; }
	inline VOID Invalidate()override { pProcedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(pProcedure)
			(*pProcedure)(Sender, Arguments...);
		}

private:
	// Common
	_func_t pProcedure;
};


//=================
// Member-Function
//=================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunction: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (_owner_t::*_func_t)();

	// Con-/Destructors
	EventMemberFunction(_owner_t* Owner, _func_t Procedure): m_Owner(Owner), pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return m_Owner; }
	inline VOID Invalidate()override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*pProcedure)();
		}

private:
	// Common
	_owner_t* m_Owner;
	_func_t pProcedure;
};


//================================
// Member-Function with Arguments
//================================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunctionWithArgs: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (_owner_t::*_func_t)(_args_t...);

	// Con-/Destructors
	EventMemberFunctionWithArgs(_owner_t* Owner, _func_t Procedure): m_Owner(Owner), pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return m_Owner; }
	inline VOID Invalidate()override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*pProcedure)(Arguments...);
		}

private:
	// Common
	_owner_t* m_Owner;
	_func_t pProcedure;
};


//=============================
// Member-Function with Sender
//=============================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunctionWithSender: public EventHandler<_sender_t, _args_t...>
{
public:
	// Definitions
	typedef VOID (_owner_t::*_func_t)(_sender_t*, _args_t...);

	// Con-/Destructors
	EventMemberFunctionWithSender(_owner_t* Owner, _func_t Procedure): m_Owner(Owner), pProcedure(Procedure) {}

	// Common
	inline VOID* GetOwner()const override { return m_Owner; }
	inline VOID Invalidate()override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*pProcedure)(Sender, Arguments...);
		}

private:
	// Common
	_owner_t* m_Owner;
	_func_t pProcedure;
};

}
