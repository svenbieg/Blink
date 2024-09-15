//==================
// UnknownClass.cpp
//==================

#include "pch.h"


//==================
// Con-/Destructors
//==================

UnknownClass::UnknownClass(UnknownClass const& value): uType(value.uType)
{
CopyMemory(pBuffer, value.pBuffer, 8);
}

UnknownClass::UnknownClass(INT value):
uType(UnknownType::Int32)
{
auto p=(INT*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(INT* value):
uType(UnknownType::Int32Ptr)
{
auto p=(INT**)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(INT64 value):
uType(UnknownType::Int64)
{
auto p=(INT64*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(INT64* value):
uType(UnknownType::Int64Ptr)
{
auto p=(INT64**)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(UINT value):
uType(UnknownType::UInt32)
{
auto p=(UINT*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(UINT* value):
uType(UnknownType::UInt32Ptr)
{
auto p=(UINT**)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(UINT64 value):
uType(UnknownType::UInt64)
{
auto p=(UINT64*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(UINT64* value):
uType(UnknownType::UInt64Ptr)
{
auto p=(UINT64**)pBuffer;
*p=value;
}

#ifndef _DRIVER
UnknownClass::UnknownClass(FLOAT value):
uType(UnknownType::Float)
{
auto p=(FLOAT*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(FLOAT* value):
uType(UnknownType::FloatPtr)
{
auto p=(FLOAT**)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(DOUBLE value):
uType(UnknownType::Double)
{
auto p=(DOUBLE*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(DOUBLE* value):
uType(UnknownType::DoublePtr)
{
auto p=(DOUBLE**)pBuffer;
*p=value;
}
#endif


UnknownClass::UnknownClass(CHAR value):
uType(UnknownType::Char)
{
auto p=(CHAR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(WCHAR value):
uType(UnknownType::WChar)
{
auto p=(WCHAR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(LPSTR value):
uType(UnknownType::String)
{
auto p=(LPSTR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(LPCSTR value):
uType(UnknownType::String)
{
auto p=(LPCSTR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(LPWSTR value):
uType(UnknownType::WString)
{
auto p=(LPWSTR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(LPCWSTR value):
uType(UnknownType::WString)
{
auto p=(LPCWSTR*)pBuffer;
*p=value;
}

UnknownClass::UnknownClass(Handle<String> value):
UnknownClass(value? value->Begin(): nullptr)
{}


//========
// Access
//========

BOOL UnknownClass::Get(INT& value)const
{
if(uType==UnknownType::Int32)
	{
	auto p=(INT*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::Int64)
	{
	auto p=(INT64*)pBuffer;
	if(*p>0x7FFFFFFF)
		return false;
	if(*p<-0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
if(uType==UnknownType::UInt32)
	{
	auto p=(UINT*)pBuffer;
	if(*p>0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
if(uType==UnknownType::UInt64)
	{
	auto p=(UINT64*)pBuffer;
	if(*p>0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(INT*& value)const
{
if(uType!=UnknownType::Int32Ptr)
	return false;
auto p=(INT**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(INT64& value)const
{
if(uType==UnknownType::Int32)
	{
	auto p=(INT*)pBuffer;
	value=(INT64)*p;
	return true;
	}
if(uType==UnknownType::Int64)
	{
	auto p=(INT64*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::UInt32)
	{
	auto p=(UINT*)pBuffer;
	value=(INT64)*p;
	return true;
	}
if(uType==UnknownType::UInt64)
	{
	auto p=(UINT64*)pBuffer;
	if(*p>0x7FFFFFFFFFFFFFFFU)
		return false;
	value=(INT64)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(INT64*& value)const
{
if(uType!=UnknownType::Int64Ptr)
	return false;
auto p=(INT64**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(UINT& value)const
{
if(uType==UnknownType::Int32)
	{
	auto p=(INT*)pBuffer;
	if(*p<0)
		return false;
	value=(UINT)*p;
	return true;
	}
if(uType==UnknownType::Int64)
	{
	auto p=(INT64*)pBuffer;
	if(*p<0)
		return false;
	if(*p>0xFFFFFFFF)
		return false;
	value=(UINT)*p;
	return true;
	}
if(uType==UnknownType::UInt32)
	{
	auto p=(UINT*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::UInt64)
	{
	auto p=(UINT64*)pBuffer;
	if(*p>0xFFFFFFFF)
		return false;
	value=(UINT)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(UINT*& value)const
{
if(uType!=UnknownType::UInt32Ptr)
	return false;
auto p=(UINT**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(UINT64& value)const
{
if(uType==UnknownType::Int32)
	{
	auto p=(INT*)pBuffer;
	if(*p<0)
		return false;
	value=*p;
	return true;
	}
if(uType==UnknownType::Int64)
	{
	auto p=(INT64*)pBuffer;
	if(*p<0)
		return false;
	value=*p;
	return true;
	}
if(uType==UnknownType::UInt32)
	{
	auto p=(UINT*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::UInt64)
	{
	auto p=(UINT64*)pBuffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(UINT64*& value)const
{
if(uType!=UnknownType::UInt64Ptr)
	return false;
auto p=(UINT64**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(FLOAT& value)const
{
if(uType!=UnknownType::Float)
	return false;
auto p=(FLOAT*)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(FLOAT*& value)const
{
if(uType!=UnknownType::FloatPtr)
	return false;
auto p=(FLOAT**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(DOUBLE& value)const
{
if(uType==UnknownType::Float)
	{
	auto p=(FLOAT*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::Double)
	{
	auto p=(DOUBLE*)pBuffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(DOUBLE*& value)const
{
if(uType!=UnknownType::DoublePtr)
	return false;
auto p=(DOUBLE**)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(CHAR& value)const
{
if(uType==UnknownType::Char)
	{
	auto p=(CHAR*)pBuffer;
	value=*p;
	return true;
	}
if(uType==UnknownType::WChar)
	{
	auto p=(WCHAR*)pBuffer;
	value=CharToAnsi(*p);
	return true;
	}
return false;
}

BOOL UnknownClass::Get(WCHAR& value)const
{
if(uType==UnknownType::Char)
	{
	auto p=(CHAR*)pBuffer;
	value=CharToUnicode(*p);
	return true;
	}
if(uType==UnknownType::WChar)
	{
	auto p=(WCHAR*)pBuffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(LPSTR& value)const
{
if(uType!=UnknownType::String)
	return false;
auto p=(LPSTR*)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(LPCSTR& value)const
{
if(uType==UnknownType::String)
	{
	auto p=(LPCSTR*)pBuffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(LPWSTR& value)const
{
if(uType!=UnknownType::WString)
	return false;
auto p=(LPWSTR*)pBuffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(LPCWSTR& value)const
{
if(uType!=UnknownType::WString)
	return false;
auto p=(LPCWSTR*)pBuffer;
value=*p;
return true;
}
