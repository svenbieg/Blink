//==========
// Global.h
//==========

#pragma once


//=======
// Using
//=======

#include "Concurrency/WriteLock.h"
#include "Handle.h"


//========
// Global
//========

class Global: public Object
{
public:
	// Using
	using Mutex=Concurrency::Mutex;
	using WriteLock=Concurrency::WriteLock;

	// Con-/Destructors
	~Global();

protected:
	// Con-/Destructors
	Global()=default;

	// Common
	template <class _obj_t, class... _args_t> static Handle<_obj_t> Create(_args_t... Arguments)
		{
		WriteLock lock(s_Mutex);
		if(s_Global)
			return s_Global;
		s_Global=Object::Create<_obj_t>(Arguments...);
		return s_Global;
		}

private:
	// Common
	static Mutex s_Mutex;
	static Global* s_Global;
};
