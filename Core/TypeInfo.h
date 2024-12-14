//============
// TypeInfo.h
//============

#pragma once


//======================
// Forward-Declarations
//======================

namespace __cxxabiv1
{
class __class_type_info;
}


//===========
// Type-Info
//===========

class TypeInfo
{
public:
	// Using
	using __class_type_info=__cxxabiv1::__class_type_info;

	// Con-/Destructors
	virtual ~TypeInfo() {}

	// Operators
	inline BOOL operator==(TypeInfo const& Info)const noexcept { return Name==Info.Name; }

	// Common
	LPCSTR Name;
	virtual BOOL TryCatch(TypeInfo const* Type, VOID** Thrown)const noexcept { return false; }
	virtual BOOL TryUpcast(__class_type_info const* Type, VOID** Object)const noexcept { return false; }
};
