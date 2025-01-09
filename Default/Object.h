//==========
// Object.h
//==========

#pragma once


//=======
// Using
//=======

#include "Culture/LanguageCode.h"
#include "TypeHelper.h"


//======================
// Forward-Declarations
//======================

class String;
template <class _obj_t> class Handle;
template <> class Handle<String>;
template <class _obj_t> class HandleBase;


//========
// Object
//========

class Object
{
public:
	// Friends
	template <class _obj_t> friend class Handle;
	template <class _obj_t> friend class HandleBase;

	// Using
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
	VOID Release();
	UINT m_RefCount;
};
