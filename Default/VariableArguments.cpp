//=======================
// VariableArguments.cpp
//=======================

#include "VariableArguments.h"


//=======
// Using
//=======

#include "StringClass.h"


//==================
// Con-/Destructors
//==================

VariableArgument::VariableArgument(Handle<String> handle):
m_Flags(VAF_CHAR|VAF_PTR|VAF_CONST),
m_Size(sizeof(TCHAR)),
m_Value(reinterpret_cast<UINT64>(handle? handle->Begin(): nullptr))
{}


//========
// Access
//========

BOOL VariableArgument::Get(Handle<String>*& Pointer)const
{
if(!FlagHelper::Get(m_Flags, VAF_STRING))
	return false;
Pointer=reinterpret_cast<Handle<String>*>(m_Value);
return true;
}


//================
// Access Private
//================

BOOL VariableArgument::GetStringPointer(BYTE flags, BYTE size, VOID** ptr)const
{
if(!BitHelper::Get(m_Flags, VAF_CHAR))
	return false;
if(m_Size!=size)
	return false;
if(BitHelper::Get(m_Flags, VAF_CONST))
	{
	if(!BitHelper::Get(flags, VAF_CONST))
		return false;
	}
if(FlagHelper::Get(m_Flags, VAF_STRING))
	{
	auto ph=reinterpret_cast<Handle<String>*>(m_Value);
	*ptr=(VOID*)(*ph)->Begin();
	return true;
	}
*ptr=(VOID*)reinterpret_cast<VOID*>(m_Value);
return true;
}


//====================
// Variable Arguments
//====================

template <class _char_t> BOOL SetString(VariableArgument const* args, UINT count, UINT* pos_ptr, _char_t const* str, UINT len)
{
auto pos=*pos_ptr;
if(pos>=count)
	return false;
Handle<String>* ph=nullptr;
if(args[pos].Get(ph))
	{
	*ph=String::Create(len, str);
	(*pos_ptr)++;
	return true;
	}
if(pos+1>=count)
	return false;
LPSTR p=nullptr;
LPWSTR pw=nullptr;
args[pos].Get(p);
args[pos].Get(pw);
if(!p&&!pw)
	return false;
UINT size=0;
args[pos+1].Get(size);
if(!size)
	return false;
StringHelper::Copy(p, size, str, len);
StringHelper::Copy(pw, size, str, len);
(*pos_ptr)+=2;
return true;
}

BOOL VariableArguments::Set(LPCSTR str, UINT len)
{
return SetString(m_Arguments, m_Count, &m_Position, str, len);
}

BOOL VariableArguments::Set(LPCWSTR str, UINT len)
{
return SetString(m_Arguments, m_Count, &m_Position, str, len);
}
