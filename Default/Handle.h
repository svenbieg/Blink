//==========
// Handle.h
//==========

#pragma once


//=======
// Using
//=======

#include "Object.h"


//============
// Base-Class
//============

template <class _obj_t> class HandleBase
{
public:
	// Friends
	template <class _friend_t> friend class HandleBase;

	// Con-/Destructors
	HandleBase(): m_Object(nullptr) {}
	HandleBase(nullptr_t): m_Object(nullptr) {}
	HandleBase(_obj_t* Object): m_Object(Object)
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	HandleBase(HandleBase const& Copy): m_Object(Copy.m_Object)
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	HandleBase(HandleBase&& Move)noexcept: m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	template <class _convert_t> HandleBase(_convert_t* Object): m_Object(static_cast<_obj_t*>(Object))
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	template <class _convert_t> HandleBase(HandleBase<_convert_t>const& Handle): m_Object(static_cast<_obj_t*>(Handle.m_Object))
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	~HandleBase() { Clear(); }

	// Access
	virtual inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator _obj_t*()const { return m_Object; }
	inline _obj_t* operator->()const { return m_Object; }

	// Comparison
	virtual inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	virtual inline BOOL operator==(_obj_t* Object)const { return m_Object==Object; }
	template <class _convert_t> inline BOOL operator==(_convert_t* Object)const { return operator==(static_cast<_obj_t*>(Object)); }
	template <class _convert_t> inline BOOL operator==(HandleBase<_convert_t> const& Handle)const { return operator==(Handle.m_Object); }
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(_obj_t* Object)const { return !operator==(Object); }
	template <class _convert_t> inline BOOL operator!=(_convert_t* Object)const { return !operator==(static_cast<_obj_t*>(Object)); }
	template <class _convert_t> inline BOOL operator!=(HandleBase<_convert_t> const& Handle)const { return !operator==(Handle.m_Object); }

	// Assignment
	inline HandleBase& operator=(nullptr_t) { Clear(); return *this; }
	inline HandleBase& operator=(_obj_t* Object) { Set(Object); return *this; }
	inline HandleBase& operator=(HandleBase const& Handle) { Set(Handle.m_Object); return *this; }
	template <class _convert_t> inline HandleBase& operator=(HandleBase<_convert_t> const& Handle) { Set(Handle.m_Object); return *this; }
	VOID Clear()
		{
		if(m_Object)
			m_Object->Release();
		m_Object=nullptr;
		}
	VOID Set(_obj_t* Object)
		{
		if(m_Object==Object)
			return;
		if(m_Object)
			m_Object->Release();
		m_Object=Object;
		if(m_Object)
			m_Object->m_RefCount++;
		}
	template <class _convert_t> inline VOID Set(_convert_t* Object) { Set(static_cast<_obj_t*>(Object)); }

protected:
	// Common
	VOID Create(Handle<_obj_t> const& Handle)
		{
		m_Object=Handle.m_Object;
		if(m_Object)
			m_Object->m_RefCount++;
		}
	template <class _convert_t> VOID Create(_convert_t* Object)
		{
		m_Object=static_cast<_obj_t*>(Object);
		if(m_Object)
			m_Object->m_RefCount++;
		}
	_obj_t* m_Object;
};


//========
// Handle
//========

template <class _obj_t>
class Handle: public HandleBase<_obj_t>
{
public:
	// Using
	using _base_t=HandleBase<_obj_t>;
	using _base_t::_base_t;

	// Access
	template <class _convert_t> inline Handle<_convert_t> As() { return dynamic_cast<_convert_t*>(_base_t::m_Object); }
};
