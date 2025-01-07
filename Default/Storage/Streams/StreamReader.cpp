//==================
// StreamReader.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Collections/list.hpp"
#include "StreamReader.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

StreamReader::StreamReader(InputStream* stream):
LastChar(0),
m_ReadAnsi(nullptr),
m_ReadUnicode(nullptr),
m_Stream(nullptr)
{
SetStream(stream);
}


//========
// Common
//========

UINT StreamReader::FindChar(LPCSTR chars)
{
UINT size=0;
while(1)
	{
	CHAR c=0;
	size+=m_ReadAnsi(m_Stream, &c);
	LastChar=CharHelper::ToChar(c);
	if(c==0)
		break;
	for(UINT u=0; chars[u]; u++)
		{
		if(c==chars[u])
			return size;
		}
	}
return size;
}

SIZE_T StreamReader::Read(VOID* buf, SIZE_T size)
{
if(!m_Stream)
	return 0;
return m_Stream->Read(buf, size);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size)
{
return DoReadString(m_ReadAnsi, buf, size);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size)
{
return DoReadString(m_ReadUnicode, buf, size);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size, CHAR esc)
{
return DoReadString(m_ReadAnsi, buf, size, esc);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size, CHAR esc)
{
return DoReadString(m_ReadUnicode, buf, size, esc);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size, LPCSTR esc, LPCSTR truncate)
{
return DoReadString(m_ReadAnsi, buf, size, esc, truncate);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size, LPCSTR esc, LPCSTR truncate)
{
return DoReadString(m_ReadUnicode, buf, size, esc, truncate);
}

Handle<String> StreamReader::ReadString(SIZE_T* size_ptr, LPCSTR esc, LPCSTR truncate)
{
#ifdef UNICODE
return DoReadString(m_ReadUnicode, size_ptr, esc, truncate);
#else
return DoReadString(m_ReadAnsi, size_ptr, esc, truncate);
#endif
}

VOID StreamReader::SetStream(InputStream* stream)
{
if(m_Stream==stream)
	return;
m_Stream=stream;
auto format=m_Stream->GetFormat();
switch(format)
	{
	case StreamFormat::Ansi:
		{
		m_ReadAnsi=CharHelper::ReadAnsi;
		m_ReadUnicode=CharHelper::ReadAnsi;
		break;
		}
	case StreamFormat::Unicode:
		{
		m_ReadAnsi=CharHelper::ReadUnicode;
		m_ReadUnicode=CharHelper::ReadUnicode;
		break;
		}
	case StreamFormat::UTF8:
		{
		m_ReadAnsi=CharHelper::ReadUtf8;
		m_ReadUnicode=CharHelper::ReadUtf8;
		break;
		}
	}
}

UINT StreamReader::Skip(UINT count)
{
UINT size_ptr=0;
for(UINT u=0; u<count; u++)
	{
	CHAR c=0;
	size_ptr+=m_ReadAnsi(m_Stream, &c);
	}
return size_ptr;
}


//================
// Common Private
//================

template <class _func_t, class _char_t> UINT StreamReader::DoReadString(_func_t read_fn, _char_t* buf, UINT size)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	read+=read_fn(m_Stream, &c);
	LastChar=CharHelper::ToChar(c);
	buf[pos++]=c;
	if(!c)
		break;
	if(pos==size)
		{
		buf[pos-1]=0;
		break;
		}
	}
return read;
}

template <class _func_t, class _char_t> UINT StreamReader::DoReadString(_func_t read_fn, _char_t* buf, UINT size, CHAR esc)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	read+=read_fn(m_Stream, &c);
	LastChar=CharHelper::ToChar(c);
	if(CharHelper::Equal(c, esc))
		c=0;
	buf[pos++]=c;
	if(!c)
		break;
	if(pos==size)
		{
		buf[pos-1]=0;
		break;
		}
	}
return read;
}

template <class _func_t, class _char_t> UINT StreamReader::DoReadString(_func_t read_fn, _char_t* buf, UINT size, LPCSTR esc, LPCSTR trunc)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	read+=read_fn(m_Stream, &c);
	LastChar=CharHelper::ToChar(c);
	BOOL skip=false;
	if(trunc)
		{
		for(UINT u=0; trunc[u]; u++)
			{
			if(CharHelper::Equal(c, trunc[u]))
				{
				skip=true;
				break;
				}
			}
		}
	if(skip)
		continue;
	trunc=nullptr;
	if(esc)
		{
		for(UINT u=0; esc[u]; u++)
			{
			if(CharHelper::Equal(c, esc[u]))
				{
				c=0;
				break;
				}
			}
		}
	buf[pos++]=c;
	if(!c)
		break;
	if(pos==size)
		{
		buf[pos-1]=0;
		break;
		}
	}
return read;
}

template <class _func_t> Handle<String> StreamReader::DoReadString(_func_t read_fn, SIZE_T* size_ptr, LPCSTR esc, LPCSTR trunc)
{
SIZE_T read=0;
Collections::list<TCHAR> buf;
while(1)
	{
	TCHAR c=0;
	read+=read_fn(m_Stream, &c);
	LastChar=c;
	BOOL skip=false;
	if(trunc)
		{
		for(UINT u=0; trunc[u]; u++)
			{
			if(CharHelper::Equal(c, trunc[u]))
				{
				skip=true;
				break;
				}
			}
		}
	if(skip)
		continue;
	trunc=nullptr;
	if(esc)
		{
		for(UINT u=0; esc[u]; u++)
			{
			if(CharHelper::Equal(c, esc[u]))
				{
				c=0;
				break;
				}
			}
		}
	if(c==0)
		break;
	buf.append(c);
	}
if(size_ptr)
	*size_ptr+=read;
UINT len=buf.get_count();
if(!len)
	return nullptr;
auto str=String::Create(len, nullptr);
LPTSTR str_ptr=const_cast<LPTSTR>(str->Begin());
buf.get_many(0, str_ptr, len);
str_ptr[len]=0;
return str;
}

}}
