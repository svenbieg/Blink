//==========
// Object.h
//==========

#pragma once


//=======
// Using
//=======

#include "Culture/LanguageCode.h"
#include "Devices/System/Cpu.h"
#include <new>


//======================
// Forward-Declarations
//======================

template <class _obj_t> class Global;

class String;
template <class _obj_t> class Handle;
template <> class Handle<String>;


//========
// Object
//========

class Object
{
public:
	// Friends
	template <class _obj_t> friend class Global;
	template <class _obj_t> friend class Handle;

	// Using
	using Cpu=Devices::System::Cpu;
	using LanguageCode=Culture::LanguageCode;

	// Con-/Destructors
	virtual ~Object() {}

	// Common
	UINT GetReferenceCount()const { return m_ReferenceCount; }
	virtual Handle<String> ToString(LanguageCode Language=LanguageCode::None);

protected:
	// Con-/Destructors
	Object(): m_ReferenceCount(0) {}
	template <class _obj_t, class... _args_t> static inline Handle<_obj_t> Create(_args_t... Arguments);
	template <class _obj_t, class... _args_t> static inline Handle<_obj_t> CreateEx(SIZE_T Extra, SIZE_T Align, _args_t... Arguments);

	// Common
	inline VOID AddReference()
		{
		Cpu::InterlockedIncrement(&m_ReferenceCount);
		}
	virtual UINT Release();
	volatile UINT m_ReferenceCount;
};


//===============
// Create Object
//===============

#include "Handle.h"

template <class _obj_t, class... _args_t> inline Handle<_obj_t> Object::Create(_args_t... Arguments)
{
auto obj=(_obj_t*)MemoryHelper::Allocate(sizeof(_obj_t));
try
	{
	new (obj) _obj_t(Arguments...);
	}
catch(Exception e)
	{
	delete obj;
	throw e;
	}
return obj;
}

template <class _obj_t, class... _args_t> inline Handle<_obj_t> Object::CreateEx(SIZE_T Extra, SIZE_T Align, _args_t... Arguments)
{
auto obj=(_obj_t*)MemoryHelper::Allocate(sizeof(_obj_t)+Extra+Align);
auto buf=(SIZE_T)obj+sizeof(_obj_t);
buf=TypeHelper::AlignUp(buf, Align);
try
	{
	new (obj) _obj_t((VOID*)buf, Extra, Arguments...);
	}
catch(Exception e)
	{
	delete obj;
	throw e;
	}
return obj;
}
