//==================
// StreamWriter.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "CharHelper.h"
#include "StreamWriter.h"



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

UINT StreamWriter::Print(LPCSTR value)
{
return DoPrint(m_WriteAnsi, 0, value);
}

UINT StreamWriter::Print(LPCSTR value, FormatFlags flags, UINT width)
{
return DoPrint(value, flags, width);
}

UINT StreamWriter::Print(LPCWSTR value)
{
return DoPrint(m_WriteUnicode, 0, value);
}

UINT StreamWriter::Print(LPCWSTR value, FormatFlags flags, UINT width)
{
return DoPrint(value, flags, width);
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

UINT StreamWriter::Print(LPCSTR str, VariableArguments const& args)
{
UINT written=0;
UINT pos=0;
UINT arg=0;
while(str[pos])
	{
	Format format=Format::None;
	FormatFlags flags=FormatFlags::None;
	UINT width=0;
	UINT prec=0;
	pos+=StringHelper::GetFormat(&str[pos], format, flags, width, prec);
	if(format==Format::None)
		{
		written+=PrintChar(str[pos++]);
		continue;
		}
	if(format==Format::Percent)
		{
		written+=PrintChar('%');
		continue;
		}
	if(FlagHelper::Get(flags, FormatFlags::Width))
		{
		if(!args.GetAt(arg++, width))
			return written;
		}
	if(FlagHelper::Get(flags, FormatFlags::Precision))
		{
		if(!args.GetAt(arg++, prec))
			return written;
		}
	switch(format)
		{
		case Format::Int:
			{
			INT64 i=0;
			if(!args.GetAt(arg++, i))
				return written;
			written+=PrintInt(i, flags, width);
			continue;
			}
		case Format::UInt:
			{
			UINT64 u=0;
			if(!args.GetAt(arg++, u))
				return written;
			written+=PrintUInt(u, flags, width);
			continue;
			}
		case Format::Hex:
			{
			UINT64 u=0;
			if(!args.GetAt(arg++, u))
				return written;
			written+=PrintHex(u, flags, width);
			continue;
			}
		case Format::Float:
			{
			FLOAT f=0;
			if(!args.GetAt(arg++, f))
				return written;
			written+=PrintFloat(f, flags, width, prec);
			continue;
			}
		case Format::Double:
			{
			DOUBLE d=0;
			if(!args.GetAt(arg++, d))
				return written;
			written+=PrintDouble(d, flags, width, prec);
			continue;
			}
		case Format::Char:
			{
			WCHAR c=0;
			if(!args.GetAt(arg++, c))
				return written;
			written+=PrintChar(c);
			continue;
			}
		case Format::String:
			{
			LPCSTR p=nullptr;
			if(args.GetAt(arg, p))
				{
				arg++;
				written+=DoPrint(p, flags, width);
				continue;
				}
			LPCWSTR pw=nullptr;
			if(args.GetAt(arg, pw))
				{
				arg++;
				written+=DoPrint(pw, flags, width);
				continue;
				}
			return written;
			}
		default:
			return written;
		}
	}
return written;
}

UINT StreamWriter::PrintChar(CHAR c, UINT count)
{
return DoPrintChar(m_WriteAnsi, c, count);
}

UINT StreamWriter::PrintChar(WCHAR c, UINT count)
{
return DoPrintChar(m_WriteUnicode, c, count);
}

UINT StreamWriter::PrintDouble(DOUBLE value, FormatFlags flags, UINT width, UINT prec)
{
CHAR buf[64];
StringHelper::PrintDouble(buf, 64, value, flags, width, prec);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintFloat(FLOAT value, FormatFlags flags, UINT width, UINT prec)
{
CHAR buf[32];
StringHelper::PrintFloat(buf, 32, value, flags, width, prec);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintHex(UINT value, FormatFlags flags, UINT width)
{
CHAR buf[32];
StringHelper::PrintHex(buf, 32, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintHex(UINT64 value, FormatFlags flags, UINT width)
{
CHAR buf[64];
StringHelper::PrintHex(buf, 64, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintInt(INT value, FormatFlags flags, UINT width)
{
CHAR buf[32];
StringHelper::PrintInt(buf, 32, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintInt(INT64 value, FormatFlags flags, UINT width)
{
CHAR buf[64];
StringHelper::PrintInt(buf, 64, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintUInt(UINT value, FormatFlags flags, UINT width)
{
CHAR buf[32];
StringHelper::PrintUInt(buf, 32, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

UINT StreamWriter::PrintUInt(UINT64 value, FormatFlags flags, UINT width)
{
CHAR buf[64];
StringHelper::PrintUInt(buf, 64, value, flags, width);
return DoPrint(m_WriteAnsi, 0, buf);
}

VOID StreamWriter::SetStream(OutputStream* stream)
{
if(m_Stream==stream)
	return;
m_Stream=stream;
auto format=m_Stream->GetFormat();
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

template <class _char_t> UINT StreamWriter::DoPrint(_char_t const* value, FormatFlags flags, UINT width)
{
if(!value)
	return 0;
UINT len=0;
if(width>0)
	len=StringHelper::Length(value);
UINT written=0;
if(len<width)
	{
	if(!FlagHelper::Get(flags, FormatFlags::Left))
		written+=PrintChar(' ', width-len);
	}
written+=Print(value);
if(len<width)
	{
	if(FlagHelper::Get(flags, FormatFlags::Left))
		written+=PrintChar(' ', width-len);
	}
return written;
}

template <class _func_t, class _char_t> UINT StreamWriter::DoPrintChar(_func_t write_fn, _char_t c, UINT count)
{
UINT size=0;
for(UINT u=0; u<count; u++)
	size+=write_fn(m_Stream, c);
return size;
}

}}
