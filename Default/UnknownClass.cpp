//==================
// UnknownClass.cpp
//==================

#include "pch.h"


//==================
// Con-/Destructors
//==================

UnknownClass::UnknownClass(UnknownClass const& value): m_BaseInfo(value.m_BaseInfo)
{
MemoryHelper::Copy(m_Buffer, value.m_Buffer, 8);
}

UnknownClass::UnknownClass(INT value):
m_BaseInfo(UnknownType::Int32)
{
auto p=(INT*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(INT* value):
m_BaseInfo(UnknownType::Int32Ptr)
{
auto p=(INT**)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(INT64 value):
m_BaseInfo(UnknownType::Int64)
{
auto p=(INT64*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(INT64* value):
m_BaseInfo(UnknownType::Int64Ptr)
{
auto p=(INT64**)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(UINT value):
m_BaseInfo(UnknownType::UInt32)
{
auto p=(UINT*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(UINT* value):
m_BaseInfo(UnknownType::UInt32Ptr)
{
auto p=(UINT**)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(UINT64 value):
m_BaseInfo(UnknownType::UInt64)
{
auto p=(UINT64*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(UINT64* value):
m_BaseInfo(UnknownType::UInt64Ptr)
{
auto p=(UINT64**)m_Buffer;
*p=value;
}

#ifndef _DRIVER
UnknownClass::UnknownClass(FLOAT value):
m_BaseInfo(UnknownType::Float)
{
auto p=(FLOAT*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(FLOAT* value):
m_BaseInfo(UnknownType::FloatPtr)
{
auto p=(FLOAT**)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(DOUBLE value):
m_BaseInfo(UnknownType::Double)
{
auto p=(DOUBLE*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(DOUBLE* value):
m_BaseInfo(UnknownType::DoublePtr)
{
auto p=(DOUBLE**)m_Buffer;
*p=value;
}
#endif


UnknownClass::UnknownClass(CHAR value):
m_BaseInfo(UnknownType::Char)
{
auto p=(CHAR*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(WCHAR value):
m_BaseInfo(UnknownType::WChar)
{
auto p=(WCHAR*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(LPSTR value):
m_BaseInfo(UnknownType::String)
{
auto p=(LPSTR*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(LPCSTR value):
m_BaseInfo(UnknownType::String)
{
auto p=(LPCSTR*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(LPWSTR value):
m_BaseInfo(UnknownType::WString)
{
auto p=(LPWSTR*)m_Buffer;
*p=value;
}

UnknownClass::UnknownClass(LPCWSTR value):
m_BaseInfo(UnknownType::WString)
{
auto p=(LPCWSTR*)m_Buffer;
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
if(m_BaseInfo==UnknownType::Int32)
	{
	auto p=(INT*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::Int64)
	{
	auto p=(INT64*)m_Buffer;
	if(*p>0x7FFFFFFF)
		return false;
	if(*p<-0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt32)
	{
	auto p=(UINT*)m_Buffer;
	if(*p>0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt64)
	{
	auto p=(UINT64*)m_Buffer;
	if(*p>0x7FFFFFFF)
		return false;
	value=(INT)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(INT*& value)const
{
if(m_BaseInfo!=UnknownType::Int32Ptr)
	return false;
auto p=(INT**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(INT64& value)const
{
if(m_BaseInfo==UnknownType::Int32)
	{
	auto p=(INT*)m_Buffer;
	value=(INT64)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::Int64)
	{
	auto p=(INT64*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt32)
	{
	auto p=(UINT*)m_Buffer;
	value=(INT64)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt64)
	{
	auto p=(UINT64*)m_Buffer;
	if(*p>0x7FFFFFFFFFFFFFFFU)
		return false;
	value=(INT64)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(INT64*& value)const
{
if(m_BaseInfo!=UnknownType::Int64Ptr)
	return false;
auto p=(INT64**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(UINT& value)const
{
if(m_BaseInfo==UnknownType::Int32)
	{
	auto p=(INT*)m_Buffer;
	if(*p<0)
		return false;
	value=(UINT)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::Int64)
	{
	auto p=(INT64*)m_Buffer;
	if(*p<0)
		return false;
	if(*p>0xFFFFFFFF)
		return false;
	value=(UINT)*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt32)
	{
	auto p=(UINT*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt64)
	{
	auto p=(UINT64*)m_Buffer;
	if(*p>0xFFFFFFFF)
		return false;
	value=(UINT)*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(UINT*& value)const
{
if(m_BaseInfo!=UnknownType::UInt32Ptr)
	return false;
auto p=(UINT**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(UINT64& value)const
{
if(m_BaseInfo==UnknownType::Int32)
	{
	auto p=(INT*)m_Buffer;
	if(*p<0)
		return false;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::Int64)
	{
	auto p=(INT64*)m_Buffer;
	if(*p<0)
		return false;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt32)
	{
	auto p=(UINT*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::UInt64)
	{
	auto p=(UINT64*)m_Buffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(UINT64*& value)const
{
if(m_BaseInfo!=UnknownType::UInt64Ptr)
	return false;
auto p=(UINT64**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(FLOAT& value)const
{
if(m_BaseInfo!=UnknownType::Float)
	return false;
auto p=(FLOAT*)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(FLOAT*& value)const
{
if(m_BaseInfo!=UnknownType::FloatPtr)
	return false;
auto p=(FLOAT**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(DOUBLE& value)const
{
if(m_BaseInfo==UnknownType::Float)
	{
	auto p=(FLOAT*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::Double)
	{
	auto p=(DOUBLE*)m_Buffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(DOUBLE*& value)const
{
if(m_BaseInfo!=UnknownType::DoublePtr)
	return false;
auto p=(DOUBLE**)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(CHAR& value)const
{
if(m_BaseInfo==UnknownType::Char)
	{
	auto p=(CHAR*)m_Buffer;
	value=*p;
	return true;
	}
if(m_BaseInfo==UnknownType::WChar)
	{
	auto p=(WCHAR*)m_Buffer;
	value=CharHelper::ToAnsi(*p);
	return true;
	}
return false;
}

BOOL UnknownClass::Get(WCHAR& value)const
{
if(m_BaseInfo==UnknownType::Char)
	{
	auto p=(CHAR*)m_Buffer;
	value=CharHelper::ToUnicode(*p);
	return true;
	}
if(m_BaseInfo==UnknownType::WChar)
	{
	auto p=(WCHAR*)m_Buffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(LPSTR& value)const
{
if(m_BaseInfo!=UnknownType::String)
	return false;
auto p=(LPSTR*)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(LPCSTR& value)const
{
if(m_BaseInfo==UnknownType::String)
	{
	auto p=(LPCSTR*)m_Buffer;
	value=*p;
	return true;
	}
return false;
}

BOOL UnknownClass::Get(LPWSTR& value)const
{
if(m_BaseInfo!=UnknownType::WString)
	return false;
auto p=(LPWSTR*)m_Buffer;
value=*p;
return true;
}

BOOL UnknownClass::Get(LPCWSTR& value)const
{
if(m_BaseInfo!=UnknownType::WString)
	return false;
auto p=(LPCWSTR*)m_Buffer;
value=*p;
return true;
}
