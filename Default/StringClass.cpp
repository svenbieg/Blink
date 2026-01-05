//=================
// StringClass.cpp
//=================

#include "StringClass.h"


//=======
// Using
//=======

#include "MemoryHelper.h"
#include <new>

using namespace Storage::Streams;


//==================
// Con-/Destructors
//==================

Handle<String> String::Create(LPCSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCWSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, LPCSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, LPCWSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCSTR Format, VariableArguments& Arguments)
{
UINT len=StringHelper::Length(Format, Arguments);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Format, Arguments);
return str;
}


//========
// Access
//========

Handle<String> String::ToString(LanguageCode lng)
{
return this;
}


//===========
// Operators
//===========

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL cs, BOOL repeat)
{
UINT len=StringHelper::Replace(nullptr, 0, m_Buffer, find, replace);
if(!len)
	return nullptr;
auto str=String::Create(len+1, nullptr);
LPTSTR buf=const_cast<LPTSTR>(str->Begin());
StringHelper::Replace(buf, len+1, m_Buffer, find, replace, cs, repeat);
return str;
}


//==========================
// Con-/Destructors Private
//==========================

String::String(LPTSTR Buffer):
m_Buffer(Buffer),
m_Hash(0),
m_Length(0)
{
m_Buffer[0]=0;
}

String::String(LPTSTR Buffer, UINT Size, LPCSTR Value):
m_Buffer(Buffer)
{
m_Length=StringHelper::Copy(m_Buffer, Size, Value);
m_Hash=StringHelper::GetHash(m_Buffer);
}

String::String(LPTSTR Buffer, UINT Size, LPCWSTR Value):
m_Buffer(Buffer)
{
m_Length=StringHelper::Copy(m_Buffer, Size, Value);
m_Hash=StringHelper::GetHash(m_Buffer);
}

String::String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments& Arguments):
m_Buffer(Buffer)
{
m_Length=StringHelper::PrintArgs(m_Buffer, Size, Format, Arguments);
m_Hash=StringHelper::GetHash(m_Buffer);
}

Handle<String> String::Create(UINT Length, nullptr_t)
{
UINT size=sizeof(String)+(Length+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf);
return str;
}
