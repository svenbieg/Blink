//==========
// Handle.h
//==========

#pragma once


//=======
// Using
//=======

#include "HandleHelper.h"


//========
// Handle
//========

template <class _obj_t>
class Handle
{
public:
	// Friends
	template <class _friend_t> friend class Handle;

	// Con-/Destructors
	Handle(): pObject(nullptr) {}
	Handle(_obj_t* Object) { HandleCreate(&pObject, Object); }
	template <class _convert_t> Handle(_convert_t* Object) { HandleCreate(&pObject, static_cast<_obj_t*>(Object)); }
	Handle(Handle<_obj_t> const& Handle) { HandleCreate(&pObject, Handle.pObject); }
	Handle(Handle<_obj_t>&& Handle)noexcept: pObject(Handle.pObject) { Handle.pObject=nullptr; }
	template <class _convert_t> Handle(Handle<_convert_t> const& Handle) { HandleCreate(&pObject, static_cast<_obj_t*>(Handle.pObject)); }
	~Handle() { HandleClear(&pObject); }

	// Access
	inline operator _obj_t*()const { return pObject; }
	inline _obj_t* operator->()const { return pObject; }
	template<class _convert_t> inline Handle<_convert_t> As()const { return dynamic_cast<_convert_t*>(pObject); }

	// Comparison
	inline BOOL operator==(_obj_t* Object)const { return pObject==Object; }
	inline BOOL operator==(nullptr_t)const { return pObject==nullptr; }
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> Object)const { return pObject==(_convert_t*)Object; }
	inline BOOL operator!=(_obj_t* Object)const { return pObject!=Object; }
	inline BOOL operator!=(nullptr_t)const { return pObject!=nullptr; }
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> Object)const { return pObject!=(_convert_t*)Object; }

	// Assignment
	inline Handle& operator=(_obj_t* Object) { HandleAssign(&pObject, Object); return *this; }
	inline Handle& operator=(Handle<_obj_t> const& Handle) { HandleAssign(&pObject, Handle.pObject); return *this; }
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Handle) { HandleAssign(&pObject, Handle.pObject); return *this; }

private:
	// Common
	_obj_t* pObject;
};
