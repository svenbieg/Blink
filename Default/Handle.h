//==========
// Handle.h
//==========

#pragma once


//=======
// Using
//=======

#include "Object.h"


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
	Handle(nullptr_t): m_Object(nullptr) {}
	Handle(_obj_t* Object): m_Object(Object)
		{
		if(m_Object)
			m_Object->m_RefCount++;
		}
	Handle(Handle const& Copy): Handle(Copy.m_Object) {}
	Handle(Handle&& Move)noexcept: m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	template <class _convert_t> Handle(_convert_t* Object): Handle(static_cast<_obj_t*>(Object)) {}
	template <class _convert_t> Handle(Handle<_convert_t>const& Copy): Handle(Copy.m_Object) {}
	~Handle()
		{
		if(m_Object)
			{
			m_Object->Release();
			m_Object=nullptr;
			}
		}

	// Access
	inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator _obj_t*()const { return m_Object; }
	inline _obj_t* operator->()const { return m_Object; }
	template <class _convert_t> inline Handle<_convert_t> As() { return dynamic_cast<_convert_t*>(m_Object); }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	inline BOOL operator==(_obj_t* Object)const { return m_Object==Object; }
	inline BOOL operator==(Handle const& Compare)const { return m_Object==Compare.m_Object; }
	template <class _convert_t> inline BOOL operator==(_convert_t* Object)const { return operator==(static_cast<_obj_t*>(Object)); }
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> const& Compare)const { return operator==(Compare.m_Object); }
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(_obj_t* Object)const { return !operator==(Object); }
	inline BOOL operator!=(Handle const& Compare)const { return !operator==(Compare.m_Object); }
	template <class _convert_t> inline BOOL operator!=(_convert_t* Object)const { return !operator==(static_cast<_obj_t*>(Object)); }
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> const& Compare)const { return !operator==(Compare.m_Object); }

	// Assignment
	inline Handle& operator=(nullptr_t)
		{
		this->~Handle();
		return *this;
		}
	Handle& operator=(_obj_t* Object)
		{
		if(m_Object==Object)
			return *this;
		if(m_Object)
			m_Object->Release();
		m_Object=Object;
		if(m_Object)
			m_Object->m_RefCount++;
		return *this;
		}
	inline Handle& operator=(Handle const& Copy) { return operator=(Copy.m_Object); }
	template <class _convert_t> inline Handle& operator=(_convert_t* Object) { return operator=(static_cast<_obj_t*>(Object)); }
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Copy) { return operator=(Copy.m_Object); }

private:
	// Common
	_obj_t* m_Object;
};
