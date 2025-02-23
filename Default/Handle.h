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
	inline Handle(): m_Object(nullptr) {}
	inline Handle(nullptr_t): m_Object(nullptr) {}
	inline Handle(_obj_t* Object) { Create(&m_Object, Object); }
	inline Handle(Handle const& Copy) { Create(&m_Object, Copy.m_Object); }
	Handle(Handle&& Move)noexcept: m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	template <class _convert_t> inline Handle(_convert_t* Object) { Create(&m_Object, Object); }
	template <class _convert_t> inline Handle(Handle<_convert_t>const& Copy) { Create(&m_Object, Copy.m_Object); }
	inline ~Handle() { Clear(&m_Object); }

	// Access
	inline operator BOOL()const { return m_Object!=nullptr; }
	inline operator _obj_t*()const { return m_Object; }
	inline _obj_t* operator->()const { return m_Object; }
	template <class _convert_t> inline Handle<_convert_t> As() { return dynamic_cast<_convert_t*>(m_Object); }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	inline BOOL operator==(_obj_t* Object)const { return m_Object==Object; }
	inline BOOL operator==(Handle const& Compare)const { return m_Object==Compare.m_Object; }
	template <class _convert_t> inline BOOL operator==(_convert_t* Object)const { return Equal(m_Object, Object); }
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> const& Compare)const { return Equal(m_Object, Compare.m_Object); }
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(_obj_t* Object)const { return !operator==(Object); }
	inline BOOL operator!=(Handle const& Compare)const { return !operator==(Compare.m_Object); }
	template <class _convert_t> inline BOOL operator!=(_convert_t* Object)const { return !Equal(m_Object, Object); }
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> const& Compare)const { return !Equal(m_Object, Compare.m_Object); }

	// Assignment
	inline Handle& operator=(nullptr_t) { Clear(&m_Object); return *this; }
	inline Handle& operator=(_obj_t* Object) { Set(&m_Object, Object); return *this; }
	inline Handle& operator=(Handle const& Copy) { Set(&m_Object, Copy.m_Object); return *this; }
	template <class _convert_t> inline Handle& operator=(_convert_t* Object) { Set(&m_Object, Object); return *this; }
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Copy) { Set(&m_Object, Copy.m_Object); return *this; }

private:
	// Common
	template <class _clear_t> static VOID Clear(_clear_t** Object)
		{
		if(*Object)
			{
			(*Object)->Release();
			*Object=nullptr;
			}
		}
	template <class _create_t, class _convert_t> static VOID Create(_create_t** Object, _convert_t* To)
		{
		auto to=static_cast<_create_t*>(To);
		if(to)
			to->Refer();
		*Object=to;
		}
	template <class _cmp_t, class _convert_t> static inline BOOL Equal(_cmp_t* Object, _convert_t* Compare)
		{
		auto cmp=static_cast<_cmp_t*>(Compare);
		return Object==cmp;
		}
	template <class _set_t, class _convert_t> static VOID Set(_set_t** Object, _convert_t* To)
		{
		auto to=static_cast<_set_t*>(To);
		if(*Object==to)
			return;
		if(*Object)
			(*Object)->Release();
		*Object=to;
		if(*Object)
			(*Object)->Refer();
		}
	_obj_t* m_Object;
};
