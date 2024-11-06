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
	Handle(): m_Object(nullptr) {}
	Handle(_obj_t* Object) { HandleCreate(&m_Object, Object); }
	template <class _convert_t> Handle(_convert_t* Object) { HandleCreate(&m_Object, static_cast<_obj_t*>(Object)); }
	Handle(Handle<_obj_t> const& Handle) { HandleCreate(&m_Object, Handle.m_Object); }
	Handle(Handle<_obj_t>&& Handle)noexcept: m_Object(Handle.m_Object) { Handle.m_Object=nullptr; }
	template <class _convert_t> Handle(Handle<_convert_t> const& Handle) { HandleCreate(&m_Object, static_cast<_obj_t*>(Handle.m_Object)); }
	~Handle() { HandleClear(&m_Object); }

	// Access
	inline operator _obj_t*()const { return m_Object; }
	inline _obj_t* operator->()const { return m_Object; }
	template<class _convert_t> inline Handle<_convert_t> As()const { return dynamic_cast<_convert_t*>(m_Object); }

	// Comparison
	inline BOOL operator==(_obj_t* Object)const { return m_Object==Object; }
	inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> Object)const { return m_Object==(_convert_t*)Object; }
	inline BOOL operator!=(_obj_t* Object)const { return m_Object!=Object; }
	inline BOOL operator!=(nullptr_t)const { return m_Object!=nullptr; }
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> Object)const { return m_Object!=(_convert_t*)Object; }

	// Assignment
	inline Handle& operator=(_obj_t* Object) { HandleAssign(&m_Object, Object); return *this; }
	inline Handle& operator=(Handle<_obj_t> const& Handle) { HandleAssign(&m_Object, Handle.m_Object); return *this; }
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Handle) { HandleAssign(&m_Object, Handle.m_Object); return *this; }

private:
	// Common
	_obj_t* m_Object;
};
