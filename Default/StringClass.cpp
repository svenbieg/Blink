//=================
// StringClass.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include <new>
#include "StringClass.h"

using namespace Storage::Streams;


//==================
// Con-/Destructors
//==================

Handle<String> String::Create(LPCSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCWSTR Value)
{
UINT len=StringHelper::Length(Value);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, LPCSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(UINT Length, LPCWSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf, len+1, Value);
return str;
}

Handle<String> String::Create(LPCSTR Format, VariableArguments const& Arguments)
{
UINT len=StringHelper::Length(Format, Arguments);
UINT size=sizeof(String)+(len+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
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


//============
// Comparison
//============

INT String::Compare(String* str1, String* str2)
{
UINT64 hash1=GetHash(str1);
UINT64 hash2=GetHash(str2);
if(hash1<hash2)
	return -1;
if(hash1>hash2)
	return 1;
if(!hash1)
	return 0;
auto buf1=str1? str1->Begin(): nullptr;
auto buf2=str2? str2->Begin(): nullptr;
return StringHelper::Compare(buf1, buf2);
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

String::String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments):
m_Buffer(Buffer)
{
m_Length=StringHelper::PrintArgs(m_Buffer, Size, Format, Arguments);
m_Hash=StringHelper::GetHash(m_Buffer);
}

Handle<String> String::Create(UINT Length, nullptr_t)
{
UINT size=sizeof(String)+(Length+1)*sizeof(TCHAR);
auto str=(String*)operator new(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf);
return str;
}
