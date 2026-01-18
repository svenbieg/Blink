//==========
// Global.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalMutex.h"
#include "Concurrency/WriteLock.h"
#include "Handle.h"


//========
// Global
//========

template <class _obj_t> class Global: public Object
{
public:
	// Using
	using CriticalMutex=Concurrency::CriticalMutex;
	using WriteLock=Concurrency::WriteLock;

	// Con-/Destructors
	~Global()
		{
		WriteLock lock(s_Mutex);
		s_Global=nullptr;
		}

protected:
	// Con-/Destructors
	Global()=default;

	// Common
	template <class... _args_t> static Handle<_obj_t> Create(_args_t... Arguments)
		{
		WriteLock lock(s_Mutex);
		if(s_Global)
			return s_Global;
		s_Global=Object::Create<_obj_t>(Arguments...);
		return s_Global;
		}

private:
	// Common
	static CriticalMutex s_Mutex;
	static _obj_t* s_Global;
};

template <class _obj_t> Concurrency::CriticalMutex Global<_obj_t>::s_Mutex;
template <class _obj_t> _obj_t* Global<_obj_t>::s_Global=nullptr;
