//==========
// Handle.h
//==========

// Smart pointer to a reference-counted object.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default/wiki#Handles

#pragma once


//=======
// Using
//=======

#include "Exception.h"
#include "MemoryHelper.h"
#include "Object.h"
#include <new>


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
	Handle()noexcept:
		m_Object(nullptr)
		{}
	inline Handle(nullptr_t)noexcept:
		m_Object(nullptr)
		{}
	inline Handle(_obj_t* Object)noexcept
		{
		Create(&m_Object, Object);
		}
	inline Handle(Handle const& Copy)noexcept
		{
		Create(&m_Object, Copy.m_Object);
		}
	inline Handle(Handle&& Move)noexcept:
		m_Object(Move.m_Object)
		{
		Move.m_Object=nullptr;
		}
	template <class _convert_t> inline Handle(_convert_t* Object)noexcept
		{
		Create(&m_Object, Object);
		}
	template <class _convert_t> inline Handle(Handle<_convert_t>const& Copy)noexcept
		{
		Create(&m_Object, Copy.m_Object);
		}
	inline ~Handle()noexcept
		{
		Clear(&m_Object);
		}

	// Access
	inline operator bool()const noexcept
		{
		return m_Object!=nullptr;
		}
	inline operator _obj_t*()const noexcept
		{
		return m_Object;
		}
	inline _obj_t* operator->()const
		{
		return m_Object;
		}
	template <class _convert_t> Handle<_convert_t> As()const
		{
		auto convert=dynamic_cast<_convert_t*>(m_Object);
		if(!convert)
			throw InvalidArgumentException();
		return convert;
		}
	template <class _convert_t> inline Handle<_convert_t> As(std::nothrow_t)const noexcept
		{
		return dynamic_cast<_convert_t*>(m_Object);
		}

	// Comparison
	inline BOOL operator==(nullptr_t)const noexcept
		{
		return m_Object==nullptr;
		}
	inline BOOL operator==(_obj_t* Compare)const noexcept
		{
		return m_Object==Compare;
		}
	inline BOOL operator==(Handle const& Compare)const noexcept
		{
		return m_Object==Compare.m_Object;
		}
	template <class _convert_t> inline BOOL operator==(_convert_t* Compare)const noexcept
		{
		return m_Object==static_cast<_obj_t*>(Compare);
		}
	template <class _convert_t> inline BOOL operator==(Handle<_convert_t> const& Compare)const noexcept
		{
		return m_Object==static_cast<_obj_t*>(Compare.m_Object);
		}
	inline BOOL operator!=(nullptr_t)const noexcept
		{
		return !operator==(nullptr);
		}
	inline BOOL operator!=(_obj_t* Compare)const noexcept
		{
		return !operator==(Compare);
		}
	inline BOOL operator!=(Handle const& Compare)const noexcept
		{
		return !operator==(Compare.m_Object);
		}
	template <class _convert_t> inline BOOL operator!=(_convert_t* Compare)const noexcept
		{
		return !operator==(Compare);
		}
	template <class _convert_t> inline BOOL operator!=(Handle<_convert_t> const& Compare)const noexcept
		{
		return !operator==(Compare);
		}
	inline BOOL operator>(nullptr_t)const noexcept
		{
		return m_Object>nullptr;
		}
	inline BOOL operator>(_obj_t* Compare)const noexcept
		{
		return m_Object>Compare;
		}
	inline BOOL operator>(Handle const& Compare)const noexcept
		{
		return m_Object>Compare.m_Object;
		}
	template <class _convert_t> inline BOOL operator>(_convert_t* Compare)const noexcept
		{
		return m_Object>Compare;
		}
	template <class _convert_t> inline BOOL operator>(Handle<_convert_t> const& Compare)const noexcept
		{
		return m_Object>Compare.m_Object;
		}
	inline BOOL operator>=(nullptr_t)const noexcept
		{
		return m_Object>=nullptr;
		}
	inline BOOL operator>=(_obj_t* Compare)const noexcept
		{
		return m_Object>=Compare;
		}
	inline BOOL operator>=(Handle const& Compare)const noexcept
		{
		return m_Object>=Compare.m_Object;
		}
	template <class _convert_t> inline BOOL operator>=(_convert_t* Compare)const noexcept
		{
		return m_Object>=Compare;
		}
	template <class _convert_t> inline BOOL operator>=(Handle<_convert_t> const& Compare)const noexcept
		{
		return m_Object>=Compare.m_Object;
		}
	inline BOOL operator<(nullptr_t)const noexcept
		{
		return false;
		}
	inline BOOL operator<(_obj_t* Compare)const noexcept
		{
		return m_Object<Compare;
		}
	inline BOOL operator<(Handle const& Compare)const noexcept
		{
		return m_Object<Compare.m_Object;
		}
	template <class _convert_t> inline BOOL operator<(_convert_t* Compare)const noexcept
		{
		return m_Object<Compare;
		}
	template <class _convert_t> inline BOOL operator<(Handle<_convert_t> const& Compare)const noexcept
		{
		return m_Object<Compare.m_Object;
		}
	inline BOOL operator<=(nullptr_t)const noexcept
		{
		return m_Object<=nullptr;
		}
	inline BOOL operator<=(_obj_t* Compare)const noexcept
		{
		return m_Object<=Compare;
		}
	inline BOOL operator<=(Handle const& Compare)const noexcept
		{
		return m_Object<=Compare.m_Object;
		}
	template <class _convert_t> inline BOOL operator<=(_convert_t* Compare)const noexcept
		{
		return m_Object<=Compare;
		}
	template <class _convert_t> inline BOOL operator<=(Handle<_convert_t> const& Compare)const noexcept
		{
		return m_Object<=Compare.m_Object;
		}

	// Assignment
	inline Handle& operator=(nullptr_t)noexcept
		{
		Clear(&m_Object);
		return *this;
		}
	inline Handle& operator=(_obj_t* Object)noexcept
		{
		Set(&m_Object, Object);
		return *this;
		}
	inline Handle& operator=(Handle const& Copy)noexcept
		{
		Set(&m_Object, Copy.m_Object);
		return *this;
		}
	template <class _convert_t> inline Handle& operator=(_convert_t* Object)noexcept
		{
		Set(&m_Object, Object);
		return *this;
		}
	template <class _convert_t> inline Handle& operator=(Handle<_convert_t> const& Copy)noexcept
		{
		Set(&m_Object, Copy.m_Object);
		return *this;
		}

private:
	// Common
	template <class _clear_t> static VOID Clear(_clear_t** Destination)noexcept
		{
		auto dst=static_cast<Object*>(*Destination);
		*Destination=nullptr;
		if(dst)
			dst->Release();
		}
	template <class _create_t, class _convert_t> static VOID Create(_create_t** Destination, _convert_t* Source)noexcept
		{
		auto src=static_cast<_create_t*>(Source);
		if(src)
			src->AddReference();
		*Destination=src;
		}
	template <class _set_t, class _convert_t> static VOID Set(_set_t** Destination, _convert_t* Source)noexcept
		{
		auto dst=static_cast<Object*>(*Destination);
		auto src=static_cast<_set_t*>(Source);
		if(src)
			src->AddReference();
		*Destination=src;
		if(dst)
			dst->Release();
		}
	_obj_t* m_Object;
};
