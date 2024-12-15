//=================
// DynamicHandle.h
//=================

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
	DynamicHandle(_owner_t* Owner, Handle<_obj_t> Object=nullptr): m_Object(Object), m_Owner(Owner) {}
	DynamicHandle(DynamicHandle const&)=delete;
	DynamicHandle(DynamicHandle&&)=delete;

	// Access
	operator bool()const { return m_Object!=nullptr; }
	operator Handle<_obj_t>() { return m_Object; }
	_obj_t* operator->()const { return m_Object; }
	Handle<_obj_t> Get()const { return m_Object; }

	// Modification
	inline DynamicHandle& operator=(nullptr_t) { return operator=((_obj_t*)nullptr); }
	inline DynamicHandle& operator=(Handle<_obj_t> Object)
		{
		Set(Object);
		return *this;
		}
	inline DynamicHandle& operator=(DynamicHandle const& Handle) { return operator=(Handle.m_Object); }
	Event<_owner_t, Handle<_obj_t>> Changed;
	VOID Set(Handle<_obj_t> Object, BOOL Notify=true)
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
