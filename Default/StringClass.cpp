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

Handle<String> String::FromInt(INT value)
{
CHAR buf[12];
StringHelper::PrintInt(buf, 12, value);
return Create(buf);
}

Handle<String> String::FromInt64(INT64 value)
{
CHAR buf[22];
StringHelper::PrintInt64(buf, 22, value);
return Create(buf);
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
auto format=stream->GetStreamFormat();
switch(format)
	{
	case StreamFormat::Ansi:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::ReadAnsi(stream, &buf[u]);
		break;
		}
	case StreamFormat::Unicode:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::ReadUnicode(stream, &buf[u]);
		break;
		}
	case StreamFormat::UTF8:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::ReadUtf8(stream, &buf[u]);
		break;
		}
	}
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
StreamFormat format=Stream::DefaultStreamFormat;
if(stream)
	format=stream->GetStreamFormat();
auto buf=str->m_Buffer;
auto len=str->m_Length;
SIZE_T size=0;
size+=Dwarf::WriteUnsigned(stream, len);
switch(format)
	{
	case StreamFormat::Ansi:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::WriteAnsi(stream, buf[u]);
		break;
		}
	case StreamFormat::Unicode:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::WriteUnicode(stream, buf[u]);
		break;
		}
	case StreamFormat::UTF8:
		{
		for(UINT u=0; u<len; u++)
			size+=CharHelper::WriteUtf8(stream, buf[u]);
		break;
		}
	}
return size;
}


//===========
// Operators
//===========

Handle<String> String::Replace(LPCSTR find, LPCSTR replace, BOOL cs, BOOL repeat)noexcept
{
UINT len=StringHelper::Replace((LPTSTR)nullptr, 0, m_Buffer, find, replace, cs, repeat);
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

String::String(LPTSTR Buffer)noexcept:
m_Buffer(Buffer),
m_Hash(0),
m_Length(0)
{
m_Buffer[0]=0;
}

String::String(LPTSTR Buffer, UINT Size, LPCSTR Value)noexcept:
m_Buffer(Buffer)
{
m_Length=StringHelper::Copy(m_Buffer, Size, Value);
m_Hash=StringHelper::Hash(m_Buffer);
}

String::String(LPTSTR Buffer, UINT Size, LPCWSTR Value)noexcept:
m_Buffer(Buffer)
{
m_Length=StringHelper::Copy(m_Buffer, Size, Value);
m_Hash=StringHelper::Hash(m_Buffer);
}

String::String(LPTSTR Buffer, UINT Size, LPCSTR Format, VariableArguments& Arguments)noexcept:
m_Buffer(Buffer)
{
m_Length=StringHelper::Print(m_Buffer, Size, Format, Arguments);
m_Hash=StringHelper::Hash(m_Buffer);
}

Handle<String> String::Create(UINT Length, nullptr_t)
{
UINT size=sizeof(String)+(Length+1)*sizeof(TCHAR);
auto str=(String*)MemoryHelper::Allocate(size);
LPTSTR buf=(LPTSTR)((SIZE_T)str+sizeof(String));
new (str) String(buf);
return str;
}
