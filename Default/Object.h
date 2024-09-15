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

	// Common
	inline VOID AddReference() { uRefCount++; }
	inline UINT GetRefCount()const { return uRefCount; }
	VOID Release();
	inline VOID RemoveReference() { uRefCount--; }
	virtual Handle<String> ToString();

protected:
	// Con-/Destructors
	Object(): uRefCount(0) {}
	virtual ~Object() {}

	// Common
	UINT uRefCount;
};
