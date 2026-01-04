//==========
// Global.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Handle.h"
#include "MemoryHelper.h"


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
	Global()=default;

	// Access
	template <class... _args_t> Handle<_obj_t> Create(_args_t... Arguments)
		{
		WriteLock lock(m_Mutex);
		if(m_Object)
			return m_Object;
		m_Object=Object::Create<_obj_t>(Arguments...);
		return m_Object;
		}

private:
	// Common
	Mutex m_Mutex;
	Handle<_obj_t> m_Object;
};
