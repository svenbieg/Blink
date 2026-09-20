//===========
// Pointer.h
//===========

#pragma once


//=======
// Using
//=======

#include "MemoryHelper.h"


//=========
// Pointer
//=========

template <class _obj_t>
class Pointer
{
public:
	// Con-/Destructors
	Pointer(): m_Object(nullptr) {}
	Pointer(nullptr_t): m_Object(nullptr) {}
	Pointer(_obj_t* Object): m_Object(Object) {}
	Pointer(Pointer const&)=delete;
	Pointer(Pointer&& Move): m_Object(Move.m_Object) { Move.m_Object=nullptr; }
	~Pointer()
		{
		if(m_Object)
			delete m_Object;
		}

	// Access
	inline operator _obj_t*()const { return m_Object; }
	inline _obj_t* operator->()const { return m_Object; }

	// Modification
	inline Pointer& operator=(nullptr_t) { Set(nullptr); return *this; }
	inline Pointer& operator=(_obj_t* Object) { Set(Object); return *this; }
	VOID Set(_obj_t* Object)
		{
		if(m_Object)
			delete m_Object;
		m_Object=Object;
		}

private:
	// Common
	_obj_t* m_Object;
};
