//============
// TypeInfo.h
//============

#pragma once


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


//==========
// Compiler
//==========

namespace std
{
typedef TypeInfo const* type_info;
}
