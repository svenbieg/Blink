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
	UINT GetReferenceCount()const { return m_ReferenceCount; }
	virtual Handle<String> ToString(LanguageCode Language=LanguageCode::None);

protected:
	// Con-/Destructors
	Object(): m_ReferenceCount(0) {}

	// Common
	inline VOID Refer()
		{
		Cpu::InterlockedIncrement(&m_ReferenceCount);
		}
	virtual UINT Release();
	volatile UINT m_ReferenceCount;
};
