//==================
// UnknownClass.cpp
//==================

#include "UnknownClass.h"


//=======
// Using
//=======

#include "MemoryHelper.h"
#include "StringClass.h"


//==================
// Con-/Destructors
//==================

UnknownClass::UnknownClass(Handle<String> handle):
#ifdef _UNICODE
m_Type(UnknownType::ConstWCharPtr),
#else
m_Type(UnknownType::ConstCharPtr),
#endif
m_Value(reinterpret_cast<UINT64>(handle? handle->Begin(): nullptr))
{}


//========
// Access
//========

BOOL UnknownClass::Get(FLOAT& value)const
{
if(m_Type!=UnknownType::Float)
	return false;
value=std::bit_cast<DOUBLE, UINT64>(m_Value);
return true;
}

BOOL UnknownClass::Get(DOUBLE& value)const
{
if(m_Type!=UnknownType::Float)
	return false;
value=std::bit_cast<DOUBLE, UINT64>(m_Value);
return true;
}

BOOL UnknownClass::Get(CHAR& value)const
{
if(m_Type!=UnknownType::Char)
	return false;
value=CharHelper::ToAnsi((WCHAR)m_Value);
return true;
}

BOOL UnknownClass::Get(WCHAR& value)const
{
if(m_Type!=UnknownType::Char)
	return false;
value=(WCHAR)m_Value;
return true;
}


//================
// Common Private
//================

BOOL UnknownClass::GetPointer(UnknownType type, VOID** ptr)const
{
if(m_Type!=type)
	return false;
*ptr=reinterpret_cast<VOID*>(m_Value);
return true;
}

BOOL UnknownClass::GetStringPointer(UnknownType type, VOID** ptr)const
{
switch(m_Type)
	{
	case UnknownType::ConstCharPtr:
		{
		if(type!=UnknownType::ConstCharPtr)
			return false;
		*ptr=reinterpret_cast<VOID*>(m_Value);
		return true;
		}
	case UnknownType::ConstWCharPtr:
		{
		if(type!=UnknownType::ConstWCharPtr)
			return false;
		*ptr=reinterpret_cast<VOID*>(m_Value);
		return true;
		}
	case UnknownType::StringHandle:
		{
		if(type==UnknownType::StringHandle)
			{
			*ptr=reinterpret_cast<VOID*>(m_Value);
			return true;
			}
		#ifdef _UNICODE
		if(type==UnknownType::ConstWCharPtr)
			{
			auto handle_ptr=reinterpret_cast<Handle<String>*>(m_Value);
			auto str=(*handle_ptr)? (*handle_ptr)->Begin(): nullptr;
			*ptr=(VOID*)str;
			return true;
			}
		#else
		if(type==UnknownType::ConstCharPtr)
			{
			auto handle_ptr=reinterpret_cast<Handle<String>*>(m_Value);
			auto str=(*handle_ptr)? (*handle_ptr)->Begin(): nullptr;
			*ptr=(VOID*)str;
			return true;
			}
		#endif
		return false;
		}
	case UnknownType::CharPtr:
		{
		BOOL str=false;
		str|=(type==UnknownType::CharPtr);
		str|=(type==UnknownType::ConstCharPtr);
		if(!str)
			return false;
		*ptr=reinterpret_cast<VOID*>(m_Value);
		return true;
		}
	case UnknownType::WCharPtr:
		{
		BOOL str=false;
		str|=(type==UnknownType::WCharPtr);
		str|=(type==UnknownType::ConstWCharPtr);
		if(!str)
			return false;
		*ptr=reinterpret_cast<VOID*>(m_Value);
		return true;
		}
	}
return false;
}
