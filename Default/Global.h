//==========
// Global.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/Mutex.h"
#include "Handle.h"


//========
// Global
//========

template <class _obj_t>
class Global
{
public:
	// Using
	using Mutex=Concurrency::Mutex;
	using WriteLock=Concurrency::WriteLock;

	// Con-/Destructors
	Global() {}

	// Access
	operator Handle<_obj_t>()
		{
		WriteLock lock(m_Mutex);
		if(m_Object)
			return m_Object;
		auto obj=(_obj_t*)operator new(sizeof(_obj_t));
		try
			{
			new (obj) _obj_t();
			}
		catch(Exception e)
			{
			operator delete(obj);
			throw e;
			}
		m_Object=obj;
		return m_Object;
		}

private:
	// Common
	Concurrency::Mutex m_Mutex;
	Handle<_obj_t> m_Object;
};
