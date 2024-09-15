//==============
// CharHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Default/CharHelper.h"
#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//======
// Ansi
//======

template <class _char_t> UINT AnsiRead(InputStream* Stream, _char_t* Char)
{
CHAR c=0;
UINT size=Stream->Read(&c, sizeof(CHAR));
*Char=CharToChar<_char_t, CHAR>(c);
return size;
}

template <class _char_t> UINT AnsiWrite(OutputStream* Stream, _char_t Char)
{
if(!Stream)
	return sizeof(CHAR);
CHAR c=CharToChar<CHAR, _char_t>(Char);
return (UINT)Stream->Write(&c, sizeof(CHAR));
}


//=========
// Unicode
//=========

template <class _char_t> UINT UnicodeRead(InputStream* Stream, _char_t* Char)
{
WCHAR c=0;
UINT size=(UINT)Stream->Read(&c, sizeof(WCHAR));
*Char=CharToChar<_char_t, WCHAR>(c);
return size;
}

template <class _char_t> UINT UnicodeWrite(OutputStream* Stream, _char_t Char)
{
if(!Stream)
	return sizeof(WCHAR);
WCHAR c=CharToChar<WCHAR, _char_t>(Char);
return (UINT)Stream->Write(&c, sizeof(WCHAR));
}


//=======
// UTF-8
//=======

template <class _char_t> UINT Utf8Read(InputStream* Stream, _char_t* Char)
{
UINT size=0;
BYTE buf[4];
UINT read=(UINT)Stream->Read(&buf[0], 1);
if(read==1)
	{
	size+=read;
	if((buf[0]&0xF0)==0xF0)
		{
		read=(UINT)Stream->Read(&buf[1], 3);
		size+=read;
		if(read==3)
			{
			*Char=CharToChar<_char_t, CHAR>('_');
			return size;
			}
		}
	else if((buf[0]&0xE0)==0xE0)
		{
		read=(UINT)Stream->Read(&buf[1], 2);
		size+=read;
		if(read==2)
			{
			*Char=CharToChar<_char_t, CHAR>('_');
			return size;
			}
		}
	else if((buf[0]&0xC0)==0xC0)
		{
		read=(UINT)Stream->Read(&buf[1], 1);
		size+=read;
		if(read==1)
			{
			WCHAR c=0;
			c|=((buf[0]&0x1F)<<8);
			c|=((buf[1]&0x3F));
			*Char=CharToChar<_char_t, WCHAR>(c);
			return size;
			}
		}
	else
		{
		CHAR c=(CHAR)(buf[0]&0x7F);
		*Char=CharToChar<_char_t, CHAR>(c);;
		return 1;
		}
	}
*Char=0;
return size;
}

template <class _char_t> UINT Utf8Write(OutputStream* Stream, _char_t Char)
{
WCHAR c=CharToChar<WCHAR, _char_t>(Char);
if(c<0x80)
	{
	if(!Stream)
		return 1;
	BYTE buf=(BYTE)c;
	return Stream->Write(&buf, 1);
	}
if(c<0x800)
	{
	if(!Stream)
		return 2;
	BYTE buf[2];
	buf[0]=((c>>6)&0x1F)|0xC0;
	buf[1]=(c&0x3F)|0x80;
	return Stream->Write(buf, 2);
	}
if(!Stream)
	return 3;
BYTE buf[3];
buf[0]=((c>>12)&0xF)|0xE0;
buf[1]=((c>>6)&0x3F)|0x80;
buf[2]=(c&0x3F)|0x80;
return Stream->Write(buf, 3);
}

}}
