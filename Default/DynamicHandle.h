//=================
// DynamicHandle.h
//=================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Event.h"


//================
// Dynamic Handle
//================

template <class _owner_t, class _obj_t>
class DynamicHandle
{
public:
	// Con-/Destructors
	DynamicHandle(_owner_t* Owner, Handle<_obj_t> Object=nullptr)noexcept:
		m_Object(Object),
		m_Owner(Owner)
		{}
	DynamicHandle(DynamicHandle const&)=delete;
	DynamicHandle(DynamicHandle&&)=delete;

	// Access
	operator bool()const noexcept
		{
		return m_Object!=nullptr;
		}
	operator _obj_t*()const noexcept
		{
		return m_Object;
		}
	operator Handle<_obj_t>()const noexcept
		{
		return m_Object;
		}
	_obj_t* operator->()const
		{
		if(!m_Object)
			throw NullPointerException();
		return m_Object;
		}
	Handle<_obj_t> Get()const noexcept 
		{
		return m_Object;
		}

	// Modification
	inline DynamicHandle& operator=(nullptr_t)
		{
		Set(nullptr);
		return *this;
		}
	inline DynamicHandle& operator=(_obj_t* Object)
		{
		Set(Object);
		return *this;
		}
	inline DynamicHandle& operator=(DynamicHandle const& Handle)
		{
		Set(Handle.m_Object);
		return *this;
		}
	inline DynamicHandle& operator=(Handle<_obj_t> const& Handle)
		{
		Set(Handle);
		return *this;
		}
	Event<_owner_t, Handle<_obj_t>> Changed;
	VOID Set(Handle<_obj_t> const& Object, BOOL Notify=true)
		{
		if(m_Object==Object)
			return;
		m_Object=Object;
		if(Notify)
			Changed(m_Owner, m_Object);
		}

private:
	// Common
	Handle<_obj_t> m_Object;
	_owner_t* m_Owner;
};
