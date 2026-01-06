//==================
// StreamWriter.cpp
//==================

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

StreamWriter::StreamWriter(OutputStream* stream):
m_Stream(nullptr),
m_WriteAnsi(nullptr),
m_WriteUnicode(nullptr)
{
SetStream(stream);
}


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
return DoPrint(m_WriteAnsi, 0, value);
}

UINT StreamWriter::Print(LPCWSTR value)
{
return DoPrint(m_WriteUnicode, 0, value);
}

UINT StreamWriter::Print(UINT len, LPCSTR value)
{
return DoPrint(m_WriteAnsi, len, value);
}

UINT StreamWriter::Print(UINT len, LPCWSTR value)
{
return DoPrint(m_WriteUnicode, len, value);
}

UINT StreamWriter::Print(Handle<String> value)
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
return DoPrintChar(m_WriteAnsi, c, count);
}

UINT StreamWriter::PrintChar(WCHAR c, UINT count)
{
return DoPrintChar(m_WriteUnicode, c, count);
}

VOID StreamWriter::SetStream(OutputStream* stream)
{
if(!stream)
	throw InvalidArgumentException();
m_Stream=stream;
auto format=m_Stream->GetStreamFormat();
SetStreamFormat(format);
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

template <class _func_t, class _char_t> UINT StreamWriter::DoPrint(_func_t write_fn, UINT len, _char_t const* value)
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
	size+=write_fn(m_Stream, value[u]);
	}
return size;
}

template <class _func_t, class _char_t> UINT StreamWriter::DoPrintChar(_func_t write_fn, _char_t c, UINT count)
{
UINT size=0;
for(UINT u=0; u<count; u++)
	size+=write_fn(m_Stream, c);
return size;
}

VOID StreamWriter::SetStreamFormat(StreamFormat format)
{
switch(format)
	{
	case StreamFormat::Ansi:
		{
		m_WriteAnsi=CharHelper::WriteAnsi;
		m_WriteUnicode=CharHelper::WriteAnsi;
		break;
		}
	case StreamFormat::Unicode:
		{
		m_WriteAnsi=CharHelper::WriteUnicode;
		m_WriteUnicode=CharHelper::WriteUnicode;
		break;
		}
	default:
	case StreamFormat::UTF8:
		{
		m_WriteAnsi=CharHelper::WriteUtf8;
		m_WriteUnicode=CharHelper::WriteUtf8;
		break;
		}
	}
}

}}