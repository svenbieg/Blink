//================
// UnknownClass.h
//================

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


//==============
// Unknown Type
//==============

enum class UnknownType
{
None,
Int32,
Int32Ptr,
Int64,
Int64Ptr,
UInt32,
UInt32Ptr,
UInt64,
UInt64Ptr,
Float,
FloatPtr,
Double,
DoublePtr,
Char,
WChar,
String,
WString
};


//===============
// Unknown Class
//===============

class UnknownClass
{
public:
	// Con-/Destructors
	UnknownClass(UnknownClass const& Value);
	UnknownClass(INT Value);
	UnknownClass(INT* Value);
	UnknownClass(INT64 Value);
	UnknownClass(INT64* Value);
	UnknownClass(UINT Value);
	UnknownClass(UINT* Value);
	UnknownClass(UINT64 Value);
	UnknownClass(UINT64* Value);
	UnknownClass(FLOAT Value);
	UnknownClass(FLOAT* Value);
	UnknownClass(DOUBLE Value);
	UnknownClass(DOUBLE* Value);
	UnknownClass(CHAR Value);
	UnknownClass(WCHAR Value);
	UnknownClass(LPSTR Value);
	UnknownClass(LPCSTR Value);
	UnknownClass(LPWSTR Value);
	UnknownClass(LPCWSTR Value);
	UnknownClass(Handle<String> Value);

	// Access
	BOOL Get(INT& Value)const;
	BOOL Get(INT*& Value)const;
	BOOL Get(INT64& Value)const;
	BOOL Get(INT64*& Value)const;
	BOOL Get(UINT& Value)const;
	BOOL Get(UINT*& Value)const;
	BOOL Get(UINT64& Value)const;
	BOOL Get(UINT64*& Value)const;
	BOOL Get(FLOAT& Value)const;
	BOOL Get(FLOAT*& Value)const;
	BOOL Get(DOUBLE& Value)const;
	BOOL Get(DOUBLE*& Value)const;
	BOOL Get(CHAR& Value)const;
	BOOL Get(WCHAR& Value)const;
	BOOL Get(LPSTR& Value)const;
	BOOL Get(LPCSTR& Value)const;
	BOOL Get(LPWSTR& Value)const;
	BOOL Get(LPCWSTR& Value)const;

private:
	// Common
	alignas(8) BYTE m_Buffer[8];
	UnknownType m_BaseInfo;
};
