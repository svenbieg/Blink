//================
// UnknownClass.h
//================

#pragma once


//=======
// Using
//=======

#include "CharHelper.h"
#include <bit>


//======================
// Forward-Declarations
//======================

class String;
template <class _obj_t> class Handle;


//==============
// Unknown Type
//==============

enum class UnknownType
{
Int,
Int16Ptr,
Int32Ptr,
Int64Ptr,
UInt8Ptr,
UInt16Ptr,
UInt32Ptr,
UInt64Ptr,
Float,
FloatPtr,
DoublePtr,
Char,
CharPtr,
ConstCharPtr,
WCharPtr,
ConstWCharPtr,
StringHandle
};


//===============
// Unknown Class
//===============

class UnknownClass
{
public:
	// Con-/Destructors
	UnknownClass(UnknownClass const& Copy)=delete;
	UnknownClass(Handle<String> Handle);
	template <std::integral _int_t> inline UnknownClass(_int_t Value): m_Type(UnknownType::Int), m_Value(Value) {}
	inline UnknownClass(FLOAT Value): m_Type(UnknownType::Float), m_Value(std::bit_cast<UINT64, DOUBLE>(Value)) {}
	inline UnknownClass(DOUBLE Value): m_Type(UnknownType::Float), m_Value(std::bit_cast<UINT64, DOUBLE>(Value)) {}
	inline UnknownClass(CHAR Value): m_Type(UnknownType::Char), m_Value(CharHelper::ToUnicode(Value)) {}
	inline UnknownClass(WCHAR Value): m_Type(UnknownType::Char), m_Value(Value) {}
	inline UnknownClass(SHORT* Pointer): m_Type(UnknownType::Int16Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(INT* Pointer): m_Type(UnknownType::Int32Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(INT64* Pointer): m_Type(UnknownType::Int64Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(BYTE* Pointer): m_Type(UnknownType::UInt8Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(WORD* Pointer): m_Type(UnknownType::UInt16Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(UINT* Pointer): m_Type(UnknownType::UInt32Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(UINT64* Pointer): m_Type(UnknownType::UInt64Ptr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(FLOAT* Pointer): m_Type(UnknownType::FloatPtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(DOUBLE* Pointer): m_Type(UnknownType::DoublePtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(LPSTR Pointer): m_Type(UnknownType::CharPtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(LPCSTR Pointer): m_Type(UnknownType::ConstCharPtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(LPWSTR Pointer): m_Type(UnknownType::WCharPtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(LPCWSTR Pointer): m_Type(UnknownType::ConstWCharPtr), m_Value(reinterpret_cast<UINT64>(Pointer)) {}
	inline UnknownClass(Handle<String>* Handle): m_Type(UnknownType::StringHandle), m_Value(reinterpret_cast<UINT64>(Handle)) {}

	// Access
	template <std::signed_integral _int_t> BOOL Get(_int_t& Value)
		{
		if(m_Type!=UnknownType::Int)
			return false;
		return TypeHelper::Signed<_int_t, UINT64>(m_Value, &Value);
		}
	template <std::unsigned_integral _uint_t> BOOL Get(_uint_t& Value)
		{
		if(m_Type!=UnknownType::Int)
			return false;
		return TypeHelper::Unsigned<_uint_t, UINT64>(m_Value, &Value);
		}
	BOOL Get(FLOAT& Value)const;
	BOOL Get(DOUBLE& Value)const;
	BOOL Get(CHAR& Value)const;
	BOOL Get(WCHAR& Value)const;
	inline BOOL Get(SHORT*& Pointer)const { return GetPointer(UnknownType::Int16Ptr, (VOID**)&Pointer); }
	inline BOOL Get(INT*& Pointer)const { return GetPointer(UnknownType::Int32Ptr, (VOID**)&Pointer); }
	inline BOOL Get(INT64*& Pointer)const { return GetPointer(UnknownType::Int64Ptr, (VOID**)&Pointer); }
	inline BOOL Get(BYTE*& Pointer)const { return GetPointer(UnknownType::UInt8Ptr, (VOID**)&Pointer); }
	inline BOOL Get(WORD*& Pointer)const { return GetPointer(UnknownType::UInt16Ptr, (VOID**)&Pointer); }
	inline BOOL Get(UINT*& Pointer)const { return GetPointer(UnknownType::UInt32Ptr, (VOID**)&Pointer); }
	inline BOOL Get(UINT64*& Pointer)const { return GetPointer(UnknownType::UInt64Ptr, (VOID**)&Pointer); }
	inline BOOL Get(FLOAT*& Pointer)const { return GetPointer(UnknownType::FloatPtr, (VOID**)&Pointer); }
	inline BOOL Get(DOUBLE*& Pointer)const { return GetPointer(UnknownType::DoublePtr, (VOID**)&Pointer); }
	inline BOOL Get(LPSTR& Pointer)const { return GetStringPointer(UnknownType::CharPtr, (VOID**)&Pointer); }
	inline BOOL Get(LPCSTR& Pointer)const { return GetStringPointer(UnknownType::ConstCharPtr, (VOID**)&Pointer); }
	inline BOOL Get(LPWSTR& Pointer)const { return GetStringPointer(UnknownType::WCharPtr, (VOID**)&Pointer); }
	inline BOOL Get(LPCWSTR& Pointer)const { return GetStringPointer(UnknownType::ConstWCharPtr, (VOID**)&Pointer); }
	inline BOOL Get(Handle<String>*& Pointer)const { return GetStringPointer(UnknownType::StringHandle, (VOID**)&Pointer); }

private:
	// Common
	BOOL GetPointer(UnknownType Type, VOID** Pointer)const;
	BOOL GetStringPointer(UnknownType Type, VOID** Pointer)const;
	UnknownType m_Type;
	UINT64 m_Value;
};
