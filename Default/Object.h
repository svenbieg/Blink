//==========
// Object.h
//==========

#pragma once


//=======
// Using
//=======

#include "Culture/LanguageCode.h"
#include "Devices/System/Cpu.h"


//======================
// Forward-Declarations
//======================

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
	template <class _obj_t> friend class Handle;

	// Using
	using Cpu=Devices::System::Cpu;
	using LanguageCode=Culture::LanguageCode;

	// Con-/Destructors
	virtual ~Object() {}

	// Common
	virtual Handle<String> ToString(LanguageCode Language=LanguageCode::None);

protected:
	// Con-/Destructors
	Object(): m_RefCount(0) {}

private:
	// Common
	inline VOID Refer()
		{
		Cpu::InterlockedIncrement(&m_RefCount);
		}
	inline VOID Release()
		{
		if(Cpu::InterlockedDecrement(&m_RefCount)==0)
			delete this;
		}
	volatile UINT m_RefCount;
};
