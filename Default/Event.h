//=========
// Event.h
//=========

#pragma once


//=======
// Using
//=======

#include "EventHandler.h"


//============
// Event-Base
//============

namespace Details {

template <class _sender_t, class... _args_t>
class EventBase
{
public:
	// Using
	using _handler_t=EventHandler<_sender_t, _args_t...>;

	// Common
	VOID Remove(VOID* Owner)
		{
		_handler_t* prev=nullptr;
		auto handler=m_Handler;
		while(handler)
			{
			auto next=handler->hNext;
			if(handler->GetOwner()!=Owner)
				{
				prev=handler;
				handler=next;
				continue;
				}
			handler->Invalidate();
			if(prev)
				{
				prev->hNext=next;
				}
			else
				{
				m_Handler=next;
				}
			handler=next;
			}
		}

protected:
	// Con-/Destructors
	EventBase() {}

	// Common
	VOID AddHandler(_handler_t* Handler)
		{
		if(!m_Handler)
			{
			m_Handler=Handler;
			return;
			}
		auto handler=m_Handler;
		while(handler->hNext)
			handler=handler->hNext;
		handler->hNext=Handler;
		}
	VOID Run(_sender_t* Sender, _args_t... Arguments)
		{
		auto handler=m_Handler;
		while(handler)
			{
			handler->Run(Sender, Arguments...);
			handler=handler->hNext;
			}
		}
	Handle<_handler_t> m_Handler;
};

}


//=======
// Event
//=======

template <class _sender_t, class... _args_t>
class Event: public ::Details::EventBase<_sender_t, _args_t...>
{
public:
	// Using
	using _handler_t=::Details::EventHandler<_sender_t, _args_t...>;

	// Con-/Destructors
	Event() {}
	Event(Event&&)=delete;
	Event(Event const&)=delete;

	// Access
	inline operator BOOL() { return this->pFirst!=nullptr; }
	VOID operator()(_sender_t* Sender, _args_t... Arguments) { this->Run(Sender, Arguments...); }

	// Modification
	inline VOID Add(VOID (*Procedure)())
		{
		_handler_t* handler=new ::Details::EventProcedure<_sender_t, _args_t...>(Procedure);
		this->AddHandler(handler);
		}
	inline VOID Add(VOID (*Procedure)(_args_t...))
		{
		_handler_t* handler=new ::Details::EventProcedureWithArgs<_sender_t, _args_t...>(Procedure);
		this->AddHandler(handler);
		}
	inline VOID Add(VOID (*Procedure)(_sender_t*, _args_t...))
		{
		_handler_t* handler=new ::Details::EventProcedureWithSender<_sender_t, _args_t...>(Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		_handler_t* handler=new ::Details::EventMemberFunction<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		_handler_t* handler=new ::Details::EventMemberFunction<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_args_t...))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithArgs<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_args_t...))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithArgs<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_sender_t*, _args_t...))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithSender<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_sender_t*, _args_t...))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithSender<_sender_t, _owner_t, _args_t...>(Owner, Procedure);
		this->AddHandler(handler);
		}
};


//=========================
// Event without Arguments
//=========================

template <class _sender_t>
class Event<_sender_t>: public ::Details::EventBase<_sender_t>
{
public:
	// Using
	using _handler_t=::Details::EventHandler<_sender_t>;

	// Con-/Destructors
	Event() {}
	Event(Event const&)=delete;
	Event(Event&&)=delete;

	// Access
	inline operator BOOL() { return this->m_Handler!=nullptr; }
	VOID operator()(_sender_t* Sender) { this->Run(Sender); }

	// Modification
	inline VOID Add(VOID (*Procedure)())
		{
		_handler_t* handler=new ::Details::EventProcedure<_sender_t>(Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)())
		{
		_handler_t* handler=new ::Details::EventMemberFunction<_sender_t, _owner_t>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)())
		{
		_handler_t* handler=new ::Details::EventMemberFunction<_sender_t, _owner_t>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(_owner_t* Owner, VOID (_owner_t::*Procedure)(_sender_t*))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithSender<_sender_t, _owner_t>(Owner, Procedure);
		this->AddHandler(handler);
		}
	template <class _owner_t> VOID Add(Handle<_owner_t> Owner, VOID (_owner_t::*Procedure)(_sender_t*))
		{
		_handler_t* handler=new ::Details::EventMemberFunctionWithSender<_sender_t, _owner_t>(Owner, Procedure);
		this->AddHandler(handler);
		}
};
