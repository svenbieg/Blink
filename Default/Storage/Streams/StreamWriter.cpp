//==================
// StreamWriter.cpp
//==================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#include "StreamWriter.h"


//=======
// Using
//=======

#include "CharHelper.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

StreamWriter::StreamWriter(OutputStream* stream)noexcept:
m_Stream(stream)
{}


//========
// Common
//========

VOID StreamWriter::Flush()
{
if(m_Stream)
	m_Stream->Flush();
}

UINT StreamWriter::Print(LPCSTR value)
{
return DoPrint(0, value);
}

UINT StreamWriter::Print(LPCWSTR value)
{
return DoPrint(0, value);
}

UINT StreamWriter::Print(UINT len, LPCSTR value)
{
return DoPrint(len, value);
}

UINT StreamWriter::Print(UINT len, LPCWSTR value)
{
return DoPrint(len, value);
}

UINT StreamWriter::Print(String const* value)
{
if(!value)
	return 0;
return Print(0, value->Begin());
}

UINT StreamWriter::Print(LPCSTR format, VariableArguments& args)
{
auto str=String::Create(format, args);
return Print(str);
}

UINT StreamWriter::PrintChar(CHAR c, UINT count)
{
return DoPrintChar(c, count);
}

UINT StreamWriter::PrintChar(WCHAR c, UINT count)
{
return DoPrintChar(c, count);
}

VOID StreamWriter::SetStream(OutputStream* stream)noexcept
{
m_Stream=stream;
}

SIZE_T StreamWriter::Write(VOID const* buf, SIZE_T size)
{
if(!m_Stream)
	return 0;
return m_Stream->Write(buf, size);
}


//================
// Common Private
//================

template <std::character _char_t> UINT StreamWriter::DoPrint(UINT len, _char_t const* value)
{
if(!value)
	return 0;
if(len==0)
	len=UINT_MAX;
UINT size=0;
for(UINT u=0; value[u]; u++)
	{
	if(u==len)
		break;
	size+=CharHelper::Write(m_Stream, value[u]);
	}
return size;
}

template <std::character _char_t> UINT StreamWriter::DoPrintChar(_char_t c, UINT count)
{
UINT size=0;
for(UINT u=0; u<count; u++)
	size+=CharHelper::Write(m_Stream, c);
return size;
}

}}