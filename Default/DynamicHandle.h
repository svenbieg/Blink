//=================
// DynamicHandle.h
//=================

#pragma once


//=======
// Using
//=======

#include "Event.h"
#include "Handle.h"


//================
// Dynamic Handle
//================

template <class _owner_t, class _obj_t>
class DynamicHandle
{
public:
	// Con-/Destructors
	DynamicHandle(_owner_t* Owner, Handle<_obj_t> Object=nullptr): hObject(Object), m_Owner(Owner) {}
	DynamicHandle(DynamicHandle const&)=delete;
	DynamicHandle(DynamicHandle&&)=delete;

	// Access
	operator bool()const { return hObject!=nullptr; }
	operator Handle<_obj_t>() { return hObject; }
	_obj_t* operator->()const { return hObject; }
	Handle<_obj_t> Get()const { return hObject; }

	// Modification
	inline DynamicHandle& operator=(nullptr_t) { return operator=((_obj_t*)nullptr); }
	inline DynamicHandle& operator=(Handle<_obj_t> Object)
		{
		Set(Object);
		return *this;
		}
	inline DynamicHandle& operator=(DynamicHandle const& Handle) { return operator=(Handle.hObject); }
	Event<_owner_t, Handle<_obj_t>> Changed;
	VOID Set(Handle<_obj_t> Object, BOOL Notify=true)
		{
		if(hObject==Object)
			return;
		hObject=Object;
		if(Notify)
			Changed(m_Owner, hObject);
		}

private:
	// Common
	Handle<_obj_t> hObject;
	_owner_t* m_Owner;
};
