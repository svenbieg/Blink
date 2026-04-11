//================
// EventHandler.h
//================

#pragma once


//=======
// Using
//=======

#include "Handle.h"


//======================
// Forward-Declarations
//======================

template <class _sender_t, class... _args_t> class Event;
template <class _sender_t, class... _args_t> class EventBase;


//===============
// Event-Handler
//===============

template <class _sender_t, class... _args_t>
class EventHandler: public Object
{
public:
	// Friends
	friend EventBase<_sender_t, _args_t...>;

	// Con-/Destructors
	virtual ~EventHandler()=default;

	// Common
	virtual VOID* GetOwner()const noexcept=0;
	virtual VOID Invalidate()noexcept=0;
	virtual VOID Run(_sender_t* Sender, _args_t... Arguments)=0;

protected:
	// Con-/Destructors
	EventHandler()=default;

	// Common
	Handle<EventHandler> m_Next;
};


//===========
// Procedure
//===========

template <class _sender_t, class... _args_t>
class EventProcedure: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (*_proc_t)();

	// Common
	inline VOID* GetOwner()const noexcept override { return (VOID*)m_Procedure; }
	inline VOID Invalidate()noexcept override { m_Procedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Procedure)
			(*m_Procedure)();
		}

private:
	// Con-/Destructors
	EventProcedure(_proc_t Procedure)noexcept: m_Procedure(Procedure) {}

	// Common
	_proc_t m_Procedure;
};


//==========================
// Procedure with Arguments
//==========================

template <class _sender_t, class... _args_t>
class EventProcedureWithArgs: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (*_proc_t)(_args_t...);

	// Common
	inline VOID* GetOwner()const noexcept override { return m_Procedure; }
	inline VOID Invalidate()noexcept override { m_Procedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Procedure)
			(*m_Procedure)(Arguments...);
		}

private:
	// Con-/Destructors
	EventProcedureWithArgs(_proc_t Procedure)noexcept: m_Procedure(Procedure) {}

	// Common
	_proc_t m_Procedure;
};


//=======================
// Procedure with Sender
//=======================

template <class _sender_t, class... _args_t>
class EventProcedureWithSender: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (*_proc_t)(_sender_t*, _args_t...);

	// Common
	inline VOID* GetOwner()const noexcept override { return m_Procedure; }
	inline VOID Invalidate()noexcept override { m_Procedure=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Procedure)
			(*m_Procedure)(Sender, Arguments...);
		}

private:
	// Con-/Destructors
	EventProcedureWithSender(_proc_t Procedure)noexcept: m_Procedure(Procedure) {}

	// Common
	_proc_t m_Procedure;
};


//=================
// Member-Function
//=================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunction: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (_owner_t::*_proc_t)();

	// Common
	inline VOID* GetOwner()const noexcept override { return m_Owner; }
	inline VOID Invalidate()noexcept override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*m_Procedure)();
		}

private:
	// Con-/Destructors
	EventMemberFunction(_owner_t* Owner, _proc_t Procedure)noexcept: m_Owner(Owner), m_Procedure(Procedure) {}

	// Common
	_owner_t* m_Owner;
	_proc_t m_Procedure;
};


//================================
// Member-Function with Arguments
//================================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunctionWithArgs: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (_owner_t::*_proc_t)(_args_t...);

	// Common
	inline VOID* GetOwner()const noexcept override { return m_Owner; }
	inline VOID Invalidate()noexcept override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*m_Procedure)(Arguments...);
		}

private:
	// Con-/Destructors
	EventMemberFunctionWithArgs(_owner_t* Owner, _proc_t Procedure)noexcept: m_Owner(Owner), m_Procedure(Procedure) {}

	// Common
	_owner_t* m_Owner;
	_proc_t m_Procedure;
};


//=============================
// Member-Function with Sender
//=============================

template <class _sender_t, class _owner_t, class... _args_t>
class EventMemberFunctionWithSender: public EventHandler<_sender_t, _args_t...>
{
public:
	// Friends
	friend Event<_sender_t, _args_t...>;

	// Definitions
	typedef VOID (_owner_t::*_proc_t)(_sender_t*, _args_t...);

	// Common
	inline VOID* GetOwner()const noexcept override { return m_Owner; }
	inline VOID Invalidate()noexcept override { m_Owner=nullptr; }
	inline VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		if(m_Owner)
			(m_Owner->*m_Procedure)(Sender, Arguments...);
		}

private:
	// Con-/Destructors
	EventMemberFunctionWithSender(_owner_t* Owner, _proc_t Procedure)noexcept: m_Owner(Owner), m_Procedure(Procedure) {}

	// Common
	_owner_t* m_Owner;
	_proc_t m_Procedure;
};
