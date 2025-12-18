//==========
// Handle.h
//==========

#pragma once


//=======
// Using
//=======

#include "Exception.h"
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
	Handle()=default;
	inline Handle(nullptr_t)noexcept: m_Object(nullptr) {}
	inline Handle(_obj_t* Object)noexcept { Create(&m_Object, Object); }
	inline Handle(Handle const& Copy)noexcept { Create(&m_Object, Copy.m_Object); }
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
	inline _obj_t* operator->()const { return Get(m_Object); }
	template <class _convert_t> inline Handle<_convert_t> As() { return dynamic_cast<_convert_t*>(m_Object); }

	// Comparison
	inline BOOL operator==(nullptr_t)const { return m_Object==nullptr; }
	inline BOOL operator==(_obj_t* Compare)const { return m_Object==Compare; }
	inline BOOL operator==(Handle const& Compare)const { return m_Object==Compare.m_Object; }
	template <class _convert_t> inline BOOL operator==(_convert_t* Compare)const { return m_Object==(_obj_t*)Compare; }
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> const& Compare)const { return m_Object==(_obj_t*)Compare.m_Object; }
	inline BOOL operator!=(nullptr_t)const { return !operator==(nullptr); }
	inline BOOL operator!=(_obj_t* Compare)const { return !operator==(Compare); }
	inline BOOL operator!=(Handle const& Compare)const { return !operator==(Compare.m_Object); }
	template <class _convert_t> inline BOOL operator!=(_convert_t* Compare)const { return m_Object!=(_obj_t*)Compare; }
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> const& Compare)const { return m_Object!=(_obj_t*)Compare.m_Object; }
	inline BOOL operator>(nullptr_t)const { return m_Object>nullptr; }
	inline BOOL operator>(_obj_t* Compare)const { return m_Object>Compare; }
	inline BOOL operator>(Handle const& Compare)const { return m_Object>Compare.m_Object; }
	template <class _convert_t> inline BOOL operator>(_convert_t* Compare)const { return m_Object>Compare; }
	template <class _convert_t> inline BOOL operator>(Handle<_convert_t> const& Compare)const { return m_Object>Compare.m_Object; }
	inline BOOL operator>=(nullptr_t)const { return m_Object>=nullptr; }
	inline BOOL operator>=(_obj_t* Compare)const { return m_Object>=Compare; }
	inline BOOL operator>=(Handle const& Compare)const { return m_Object>=Compare.m_Object; }
	template <class _convert_t> inline BOOL operator>=(_convert_t* Compare)const { return m_Object>=Compare; }
	template <class _convert_t> inline BOOL operator>=(Handle<_convert_t> const& Compare)const { return m_Object>=Compare.m_Object; }
	inline BOOL operator<(nullptr_t)const { return false; }
	inline BOOL operator<(_obj_t* Compare)const { return m_Object<Compare; }
	inline BOOL operator<(Handle const& Compare)const { return m_Object<Compare.m_Object; }
	template <class _convert_t> inline BOOL operator<(_convert_t* Compare)const { return m_Object<Compare; }
	template <class _convert_t> inline BOOL operator<(Handle<_convert_t> const& Compare)const { return m_Object<Compare.m_Object; }
	inline BOOL operator<=(nullptr_t)const { return m_Object<=nullptr; }
	inline BOOL operator<=(_obj_t* Compare)const { return m_Object<=Compare; }
	inline BOOL operator<=(Handle const& Compare)const { return m_Object<=Compare.m_Object; }
	template <class _convert_t> inline BOOL operator<=(_convert_t* Compare)const { return m_Object<=Compare; }
	template <class _convert_t> inline BOOL operator<=(Handle<_convert_t> const& Compare)const { return m_Object<=Compare.m_Object; }

	// Assignment
	inline Handle& operator=(nullptr_t) { Clear(&m_Object); return *this; }
	inline Handle& operator=(_obj_t* Object) { Set(&m_Object, Object); return *this; }
	inline Handle& operator=(Handle const& Copy) { Set(&m_Object, Copy.m_Object); return *this; }
	template <class _convert_t> inline Handle& operator=(_convert_t* Object) { Set(&m_Object, Object); return *this; }
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Copy) { Set(&m_Object, Copy.m_Object); return *this; }

private:
	// Common
	template <class _clear_t> static VOID Clear(_clear_t** Clear)
		{
		auto obj=static_cast<Object*>(*Clear);
		*Clear=nullptr;
		if(obj)
			obj->Release();
		}
	template <class _create_t, class _convert_t> static VOID Create(_create_t** Create, _convert_t* Init)noexcept
		{
		auto init=static_cast<_create_t*>(Init);
		if(init)
			init->AddReference();
		*Create=init;
		}
	template <class _get_t> static inline _get_t* Get(_get_t* Get)
		{
		if(!Get)
			throw NullPointerException();
		return Get;
		}
	template <class _set_t, class _convert_t> static VOID Set(_set_t** Set, _convert_t* To)
		{
		if(*Set==To)
			return;
		auto obj=static_cast<Object*>(*Set);
		*Set=static_cast<_set_t*>(To);
		if(obj)
			obj->Release();
		if(To)
			To->AddReference();
		}
	_obj_t* m_Object=nullptr;
};
