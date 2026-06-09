//================
// EventHandler.h
//================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default/wiki#Events

#pragma once


//=======
// Using
//=======

#include "FlagHelper.h"


//======================
// Forward-Declarations
//======================

template <class _sender_t, class... _args_t> class Event;
template <class _sender_t, class... _args_t> class EventBase;


//===============
// Event-Handler
//===============

template <class _sender_t, class... _args_t>
class EventHandler
{
public:
	// Friends
	friend EventBase<_sender_t, _args_t...>;

	// Con-/Destructors
	virtual ~EventHandler()=default;

	// Common
	virtual VOID* GetOwner()const noexcept=0;
	virtual VOID Invalidate()noexcept=0;
	BOOL IsRunning()const noexcept
		{
		return FlagHelper::Get(m_Flags, EventHandlerFlags::Running);
		}
	virtual BOOL IsValid()const noexcept=0;
	virtual VOID Run(_sender_t* Sender, _args_t... Arguments)=0;

protected:
	// Flags
	enum class EventHandlerFlags: UINT
		{
		None=0,
		Running=1
		};

	// Con-/Destructors
	EventHandler()noexcept:
		m_Flags(EventHandlerFlags::None),
		m_Next(nullptr)
		{}

	// Common
	EventHandlerFlags m_Flags;
	EventHandler* m_Next;
};


//===========
// Procedure
//===========

template <class _sender_t, class... _args_t>
class EventProcedure: public EventHandler<_sender_t, _args_t...>
{
public:
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (*_proc_t)();

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return (VOID*)m_Procedure;
		}
	VOID Invalidate()noexcept override
		{
		m_Procedure=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Procedure!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Procedure)
			(*m_Procedure)();
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventProcedure(_proc_t Procedure)noexcept:
		m_Procedure(Procedure)
		{}

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
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (*_proc_t)(_args_t...);

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return m_Procedure;
		}
	VOID Invalidate()noexcept override
		{
		m_Procedure=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Procedure!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Procedure)
			(*m_Procedure)(Arguments...);
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventProcedureWithArgs(_proc_t Procedure)noexcept:
		m_Procedure(Procedure)
		{}

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
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (*_proc_t)(_sender_t*, _args_t...);

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return m_Procedure;
		}
	VOID Invalidate()noexcept override
		{
		m_Procedure=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Procedure!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Procedure)
			(*m_Procedure)(Sender, Arguments...);
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventProcedureWithSender(_proc_t Procedure)noexcept:
		m_Procedure(Procedure)
		{}

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
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (_owner_t::*_proc_t)();

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return m_Owner;
		}
	VOID Invalidate()noexcept override
		{
		m_Owner=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Owner!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Owner)
			(m_Owner->*m_Procedure)();
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventMemberFunction(_owner_t* Owner, _proc_t Procedure)noexcept:
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

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
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (_owner_t::*_proc_t)(_args_t...);

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return m_Owner;
		}
	VOID Invalidate()noexcept override
		{
		m_Owner=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Owner!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Owner)
			(m_Owner->*m_Procedure)(Arguments...);
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventMemberFunctionWithArgs(_owner_t* Owner, _proc_t Procedure)noexcept:
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

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
	// Using
	using _base_t=EventHandler<_sender_t, _args_t...>;
	typedef VOID (_owner_t::*_proc_t)(_sender_t*, _args_t...);

	// Friends
	friend Event<_sender_t, _args_t...>;

	// Common
	VOID* GetOwner()const noexcept override
		{
		return m_Owner;
		}
	VOID Invalidate()noexcept override
		{
		m_Owner=nullptr;
		}
	BOOL IsValid()const noexcept override
		{
		return m_Owner!=nullptr;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)override
		{
		FlagHelper::Set(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		if(m_Owner)
			(m_Owner->*m_Procedure)(Sender, Arguments...);
		FlagHelper::Clear(_base_t::m_Flags, _base_t::EventHandlerFlags::Running);
		}

private:
	// Con-/Destructors
	EventMemberFunctionWithSender(_owner_t* Owner, _proc_t Procedure)noexcept:
		m_Owner(Owner),
		m_Procedure(Procedure)
		{}

	// Common
	_owner_t* m_Owner;
	_proc_t m_Procedure;
};
