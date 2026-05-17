//==================
// StreamReader.cpp
//==================

#include "StreamReader.h"


//=======
// Using
//=======

#include "StringBuilder.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

StreamReader::StreamReader(InputStream* stream)noexcept:
LastChar(0),
m_Stream(stream)
{}


//========
// Common
//========

SIZE_T StreamReader::Available()
{
if(!m_Stream)
	return 0;
return m_Stream->Available();
}

UINT StreamReader::FindChar(LPCSTR chars)
{
UINT size=0;
while(1)
	{
	WCHAR c=0;
	UINT read=CharHelper::Read(m_Stream, &c);
	if(!read)
		break;
	size+=read;
	LastChar=CharHelper::ToCharT<TCHAR, WCHAR>(c);
	if(c==0)
		break;
	for(UINT u=0; chars[u]; u++)
		{
		if(CharHelper::Equal(c, chars[u]))
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

UINT StreamReader::ReadChar(CHAR* c_ptr)
{
return CharHelper::Read(m_Stream, c_ptr);
}

UINT StreamReader::ReadChar(WCHAR* c_ptr)
{
return CharHelper::Read(m_Stream, c_ptr);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size)
{
return DoReadString(buf, size);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size)
{
return DoReadString(buf, size);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size, CHAR esc)
{
return DoReadString(buf, size, esc);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size, CHAR esc)
{
return DoReadString(buf, size, esc);
}

UINT StreamReader::ReadString(LPSTR buf, UINT size, LPCSTR esc, LPCSTR truncate)
{
return DoReadString(buf, size, esc, truncate);
}

UINT StreamReader::ReadString(LPWSTR buf, UINT size, LPCSTR esc, LPCSTR truncate)
{
return DoReadString(buf, size, esc, truncate);
}

Handle<String> StreamReader::ReadString(SIZE_T* size_ptr, LPCSTR esc, LPCSTR truncate)
{
return DoReadString(size_ptr, esc, truncate);
}

VOID StreamReader::SetStream(InputStream* stream)noexcept
{
m_Stream=stream;
}

UINT StreamReader::Skip(UINT count)
{
UINT size=0;
for(UINT u=0; u<count; u++)
	{
	UINT read=CharHelper::Read(m_Stream);
	if(!read)
		break;
	size+=read;
	}
return size;
}

UINT StreamReader::Skip(LPCSTR chars)
{
UINT size=0;
while(1)
	{
	WCHAR c=0;
	UINT read=CharHelper::Read(m_Stream, &c);
	if(!read)
		break;
	size+=read;
	BOOL found=false;
	for(UINT u=0; chars[u]; u++)
		{
		if(CharHelper::Equal(c, chars[u]))
			{
			found=true;
			break;
			}
		}
	if(found)
		break;
	}
return size;
}


//================
// Common Private
//================

template <std::character _char_t> UINT StreamReader::DoReadString(_char_t* buf, UINT size)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	UINT rd=CharHelper::Read(m_Stream, &c);
	LastChar=CharHelper::ToCharT<TCHAR>(c);
	if(!rd)
		break;
	read+=rd;
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

template <std::character _char_t> UINT StreamReader::DoReadString(_char_t* buf, UINT size, CHAR esc)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	UINT rd=CharHelper::Read(m_Stream, &c);
	LastChar=CharHelper::ToCharT<TCHAR, _char_t>(c);
	if(!rd)
		break;
	read+=rd;
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

template <std::character _char_t> UINT StreamReader::DoReadString(_char_t* buf, UINT size, LPCSTR esc, LPCSTR trunc)
{
UINT read=0;
UINT pos=0;
while(1)
	{
	_char_t c=0;
	UINT rd=CharHelper::Read(m_Stream, &c);
	LastChar=CharHelper::ToCharT<TCHAR, _char_t>(c);
	if(!rd)
		break;
	read+=rd;
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

Handle<String> StreamReader::DoReadString(SIZE_T* size_ptr, LPCSTR esc, LPCSTR trunc)
{
StringBuilder builder;
SIZE_T read=0;
while(1)
	{
	TCHAR c=0;
	UINT rd=CharHelper::Read(m_Stream, &c);
	LastChar=c;
	if(!rd)
		break;
	read+=rd;
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
	builder.Append(c);
	}
if(size_ptr)
	*size_ptr+=read;
return builder.ToString();
}

}}