//=================
// StringClass.cpp
//=================

#include "StringClass.h"


//=======
// Using
//=======

#include "Storage/Encoding/Dwarf.h"
#include "MemoryHelper.h"
#include <new>

using namespace Storage::Encoding;
using namespace Storage::Streams;


//============
// Conversion
//============

template <std::integral _int_t>
VOID StringFromInt(LPTSTR dst, UINT size, _int_t value)noexcept
{
BOOL sign=value<0;
if(sign)
	value=-value;
LPTSTR buf=&dst[size-1];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
if(sign)
	*--buf='-';
}

template <std::integral _int_t>
UINT StringFromIntLength(_int_t value)noexcept
{
UINT len=0;
if(value<0)
	{
	len++;
	value=-value;
	}
do
	{
	value/=10;
	len++;
	}
while(value);
return len;
}

template <std::unsigned_integral _uint_t>
VOID StringFromUInt(LPTSTR dst, UINT size, _uint_t value)noexcept
{
LPTSTR buf=&dst[size-1];
*buf=0;
do
	{
	*--buf=(CHAR)((value%10)+'0');
	value/=10;
	}
while(value);
}

template <std::unsigned_integral _uint_t>
UINT StringFromUIntLength(_uint_t value)noexcept
{
UINT len=0;
do
	{
	value/=10;
	len++;
	}
while(value);
return len;
}


//==================
// Con-/Destructors
//==================

Handle<String> String::Create(INT value)
{
UINT len=StringFromIntLength(value);
auto str=String::Create(len, nullptr);
StringFromInt(str->m_Buffer, len+1, value);
str->m_Length=len;
return str;
}

Handle<String> String::Create(INT64 value)
{
UINT len=StringFromIntLength(value);
auto str=String::Create(len, nullptr);
StringFromInt(str->m_Buffer, len+1, value);
str->m_Length=len;
return str;
}

Handle<String> String::Create(UINT value)
{
UINT len=StringFromUIntLength(value);
auto str=String::Create(len, nullptr);
StringFromUInt(str->m_Buffer, len+1, value);
str->m_Length=len;
return str;
}

Handle<String> String::Create(UINT64 value)
{
UINT len=StringFromUIntLength(value);
auto str=String::Create(len, nullptr);
StringFromUInt(str->m_Buffer, len+1, value);
str->m_Length=len;
return str;
}

Handle<String> String::Create(LPCSTR Value)
{
UINT len=StringHelper::Length(Value);
auto str=String::Create(len, nullptr);
StringHelper::Copy(str->m_Buffer, len+1, Value);
str->m_Hash=StringHelper::Hash(str->m_Buffer);
str->m_Length=len;
return str;
}

Handle<String> String::Create(LPCWSTR Value)
{
UINT len=StringHelper::Length(Value);
auto str=String::Create(len, nullptr);
StringHelper::Copy(str->m_Buffer, len+1, Value);
str->m_Hash=StringHelper::Hash(str->m_Buffer);
str->m_Length=len;
return str;
}

Handle<String> String::Create(UINT Length, LPCSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
auto str=String::Create(len, nullptr);
StringHelper::Copy(str->m_Buffer, len+1, Value, len);
str->m_Hash=StringHelper::Hash(str->m_Buffer);
str->m_Length=len;
return str;
}

Handle<String> String::Create(UINT Length, LPCWSTR Value)
{
UINT len=StringHelper::Length(Value, Length);
auto str=String::Create(len, nullptr);
StringHelper::Copy(str->m_Buffer, len+1, Value, len);
str->m_Hash=StringHelper::Hash(str->m_Buffer);
str->m_Length=len;
return str;
}

Handle<String> String::Create(LPCSTR Format, VariableArguments& Arguments)
{
UINT len=StringHelper::Length(Format, Arguments);
auto str=String::Create(len, nullptr);
StringHelper::Print(str->m_Buffer, len+1, Format, Arguments);
str->m_Hash=StringHelper::Hash(str->m_Buffer);
str->m_Length=len;
return str;
}

Handle<String> String::ReadFromStream(InputStream* stream, SIZE_T* size_ptr)
{
if(!stream)
	throw InvalidArgumentException();
SIZE_T size=0;
UINT len=0;
size+=Dwarf::ReadUnsigned(stream, &len);
if(!len)
	{
	if(size_ptr)
		*size_ptr+=size;
	return nullptr;
	}
auto str=Create(len, nullptr);
auto buf=str->m_Buffer;
size+=stream->Read(buf, len);
buf[len]=0;
str->m_Hash=StringHelper::Hash(buf);
str->m_Length=len;
if(size_ptr)
	*size_ptr+=size;
return str;
}


//========
// Access
//========

Handle<String> String::ToString(LanguageCode lng)
{
return this;
}

SIZE_T String::WriteToStream(String const* str, OutputStream* stream)
{
if(!str)
	return Dwarf::WriteUnsigned(stream, 0U);
auto buf=str->m_Buffer;
auto len=str->m_Length;
SIZE_T size=0;
size+=Dwarf::WriteUnsigned(stream, len);
size+=OutputStream::Write(stream, buf, len);
return size;
}


//===========
// Operators
//===========

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL repeat)
{
UINT len=StringHelper::Replace((LPTSTR)nullptr, 0, m_Buffer, find, replace, repeat);
if(!len)
	return nullptr;
auto str=String::Create(len, nullptr);
auto buf=str->m_Buffer;
str->m_Length=StringHelper::Replace(buf, len+1, m_Buffer, find, replace, repeat);
str->m_Hash=StringHelper::Hash(buf);
return str;
}

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL repeat, CompareMode mode)
{
UINT len=StringHelper::Replace((LPTSTR)nullptr, 0, m_Buffer, find, replace, repeat, mode);
if(!len)
	return nullptr;
auto str=String::Create(len, nullptr);
auto buf=str->m_Buffer;
str->m_Length=StringHelper::Replace(buf, len+1, m_Buffer, find, replace, repeat, mode);
str->m_Hash=StringHelper::Hash(buf);
return str;
}


//==========================
// Con-/Destructors Private
//==========================

String::String(LPTSTR Buffer)noexcept:
m_Buffer(Buffer),
m_Hash(0),
m_Length(0)
{
m_Buffer[0]=0;
}

Handle<String> String::Create(UINT Length, nullptr_t)
{
UINT size=sizeof(String)+(Length+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf);
return str;
}
