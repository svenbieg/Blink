//============
// TypeInfo.h
//============

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Type-Info
//===========

class TypeInfo
{
public:
	// Con-/Destructors
	virtual ~TypeInfo() {}

	// Operators
	inline BOOL operator==(TypeInfo const& Info)const noexcept { return Name==Info.Name; }

	// Common
	LPCSTR Name;
	virtual BOOL TryUpcast(TypeInfo const* Type, VOID** Object)const noexcept { return false; }
};
