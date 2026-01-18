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

template <class _obj_t> class Global: public Object
{
public:
	// Using
	using Cpu=Devices::System::Cpu;
	using Mutex=Concurrency::Mutex;
	using WriteLock=Concurrency::WriteLock;

protected:
	// Con-/Destructors
	Global()=default;

	// Common
	template <class... _args_t> static _obj_t* Create(_args_t... Arguments)
		{
		WriteLock lock(s_Mutex);
		if(s_Global)
			return s_Global;
		s_Global=Object::Create<_obj_t>(Arguments...);
		return s_Global;
		}
	virtual UINT Release()override
		{
		WriteLock lock(s_Mutex);
		UINT ref_count=Cpu::InterlockedDecrement(&m_ReferenceCount);
		if(ref_count==0)
			{
			delete this;
			s_Global=nullptr;
			}
		return ref_count;
		}

private:
	// Common
	static Mutex s_Mutex;
	static _obj_t* s_Global;
};

template <class _obj_t> Concurrency::Mutex Global<_obj_t>::s_Mutex;
template <class _obj_t> _obj_t* Global<_obj_t>::s_Global=nullptr;
