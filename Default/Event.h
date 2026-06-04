//=========
// Event.h
//=========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default/wiki#Events

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "EventHandler.h"


//============
// Event-Base
//============

template <class _sender_t, class... _args_t>
class EventBase
{
public:
	// Using
	using _handler_t=EventHandler<_sender_t, _args_t...>;
	using Task=Concurrency::Task;

	// Common
	VOID Remove(VOID* Owner)noexcept
		{
		assert(Task::IsMainTask());
		_handler_t** current_ptr=&m_Handler;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			if(current->GetOwner()==Owner)
				{
				if(current->IsRunning())
					{
					current->Invalidate();
					}
				else
					{
					*current_ptr=current->m_Next;
					delete current;
					continue;
					}
				}
			current_ptr=&current->m_Next;
			}
		}

protected:
	// Con-/Destructors
	EventBase(): m_Handler(nullptr) {}

	// Common
	VOID AddHandler(_handler_t* Handler)noexcept
		{
		assert(Task::IsMainTask());
		auto current_ptr=&m_Handler;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			current_ptr=&current->m_Next;
			}
		*current_ptr=Handler;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)
		{
		assert(Task::IsMainTask());
		auto current_ptr=&m_Handler;
		while(*current_ptr)
			{
			auto current=*current_ptr;
			current->Run(Sender, Arguments...);
			if(!current->IsValid())
				{
				*current_ptr=current->m_Next;
				delete current;
				continue;
				}
			current_ptr=&current->m_Next;
			}
		}
	_handler_t* m_Handler;
};


//=======
// Event
//=======

template <class _sender_t, class... _args_t>
class Event: public EventBase<_sender_t, _args_t...>
{
public:
	// Using
	using _base_t=EventBase<_sender_t, _args_t...>;
	using _handler_t=EventHandler<_sender_t, _args_t...>;

	// Con-/Destructors
	Event()=default;
	Event(Event&&)=delete;
	Event(Event const&)=delete;

	// Access
	inline operator BOOL() { return _base_t::m_Handler!=nullptr; }
	inline VOID operator()(_sender_t* Sender, _args_t... Arguments) { _base_t::Run(Sender, Arguments...); }

	// Modification
	inline VOID Add(VOID (*Procedure)())
		{
		auto handler=new EventProcedure<_sender_t, _args_t...>(Procedure);
		_base_t::AddHandler(handler);
		}
	inline VOID Add(VOID (*Procedure)(_args_t...))
		{
		auto handler=new EventProcedureWithArgs<_sender_t, _args_t...>(Procedure);
		_base_t::AddHandler(handler);
		}
	inline VOID Add(VOID (*Procedure)(_sender_t*, _args_t...))
		{
		auto handler=new EventProcedureWithSender<_sender_t, _args_t...>(Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new EventMemberFunction<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new EventMemberFunction<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_args_t...))
		{
		auto handler=new EventMemberFunctionWithArgs<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_args_t...))
		{
		auto handler=new EventMemberFunctionWithArgs<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_sender_t*, _args_t...))
		{
		auto handler=new EventMemberFunctionWithSender<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_sender_t*, _args_t...))
		{
		auto handler=new EventMemberFunctionWithSender<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
};


//=========================
// Event without Arguments
//=========================

template <class _sender_t>
class Event<_sender_t>: public EventBase<_sender_t>
{
public:
	// Using
	using _base_t=EventBase<_sender_t>;
	using _handler_t=EventHandler<_sender_t>;

	// Con-/Destructors
	Event()=default;
	Event(Event const&)=delete;
	Event(Event&&)=delete;

	// Access
	inline operator BOOL() { return _base_t::m_Handler!=nullptr; }
	inline VOID operator()(_sender_t* Sender) { _base_t::Run(Sender); }

	// Modification
	inline VOID Add(VOID (*Procedure)())
		{
		auto handler=new EventProcedure<_sender_t>(Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new EventMemberFunction<_sender_t, _owner_t>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		auto handler=new EventMemberFunction<_sender_t, _owner_t>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_sender_t*))
		{
		auto handler=new EventMemberFunctionWithSender<_sender_t, _owner_t>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
	template <class _owner_t> inline VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_sender_t*))
		{
		auto handler=new EventMemberFunctionWithSender<_sender_t, _owner_t>(Owner, Procedure);
		_base_t::AddHandler(handler);
		}
};
