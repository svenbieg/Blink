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


//========
// Object
//========

class Object
{
public:
	// Friends
	template <class _item_t> friend class Handle;

	// Con-/Destructors
	virtual ~Object() {}

	// Common
	inline VOID AddReference() { m_RefCount++; }
	inline UINT GetRefCount()const { return m_RefCount; }
	VOID Release();
	inline VOID RemoveReference() { m_RefCount--; }
	virtual Handle<String> ToString();

protected:
	// Con-/Destructors
	Object(): m_RefCount(0) {}

	// Common
	UINT m_RefCount;
};
