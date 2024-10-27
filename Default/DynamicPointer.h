//==================
// DynamicPointer.h
//==================

#pragma once


//=======
// Using
//=======

#include "Event.h"
#include "Handle.h"


//=================
// Dynamic Pointer
//=================

template <class _owner_t, class _obj_t>
class DynamicPointer
{
public:
	// Con-/Destructors
	DynamicPointer(_owner_t* Owner, _obj_t* Object=nullptr): pObject(Object), m_Owner(Owner) {}
	DynamicPointer(DynamicPointer const&)=delete;
	DynamicPointer(DynamicPointer&&)=delete;

	// Access
	operator bool()const { return pObject!=nullptr; }
	operator _obj_t*()const { return pObject; }
	_obj_t* operator->()const { return pObject; }
	_obj_t* Get()const { return pObject; }

	// Modification
	inline DynamicPointer& operator=(nullptr_t) { return operator=((_obj_t*)nullptr); }
	inline DynamicPointer& operator=(_obj_t* Object)
		{
		Set(Object);
		return *this;
		}
	inline DynamicPointer& operator=(DynamicPointer const& Pointer) { return operator=(Pointer.pObject); }
	Event<_owner_t, _obj_t*> Changed;
	VOID Set(_obj_t* Object, BOOL Notify=true)
		{
		if(pObject==Object)
			return;
		pObject=Object;
		if(Notify)
			Changed(m_Owner, pObject);
		}

private:
	// Common
	_obj_t* pObject;
	_owner_t* m_Owner;
};
