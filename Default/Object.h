//==========
// Object.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//======================
// Forward-Declarations
//======================

class String;
template <class _obj_t> class Handle;
template <> class Handle<String>;

namespace Details
{
template <class _obj_t> class HandleBase;
}


//========
// Object
//========

class Object
{
public:
	// Friends
	template <class _obj_t> friend class ::Details::HandleBase;

	// Con-/Destructors
	virtual ~Object() {}

	// Common
	virtual Handle<String> ToString();

protected:
	// Con-/Destructors
	Object(): m_RefCount(0) {}

private:
	// Common
	VOID Release();
	UINT m_RefCount;
};
