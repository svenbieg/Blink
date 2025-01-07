//============
// Buffer.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Buffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Buffer::~Buffer()
{
if(m_Options!=BufferOptions::Static)
	delete m_Buffer;
}

Handle<Buffer> Buffer::Create(SIZE_T size)
{
return new Buffer(size);
}

Handle<Buffer> Buffer::Create(VOID const* buf, SIZE_T size, BufferOptions options)
{
return new Buffer(buf, size, options);
}


//==============
// Input-Stream
//==============

SIZE_T Buffer::Available()
{
if(m_Size==0)
	return 0;
return m_Size-m_Position;
}

SIZE_T Buffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=SIZE_MAX;
if(m_Size!=0)
	available=m_Size-m_Position;
SIZE_T copy=TypeHelper::Min(size, available);
if(buf)
	MemoryHelper::Copy(buf, &m_Buffer[m_Position], copy);
m_Position+=copy;
return copy;
}


//===============
// Output-Stream
//===============

SIZE_T Buffer::Write(VOID const* buf, SIZE_T size)
{
if(m_Size==0)
	return 0;
SIZE_T available=m_Size-m_Position;
SIZE_T copy=TypeHelper::Min(size, available);
MemoryHelper::Copy(&m_Buffer[m_Position], buf, copy);
m_Position+=copy;
return copy;
}


//==========
// Seekable
//==========

BOOL Buffer::Seek(FILE_SIZE pos)
{
if(m_Size&&pos>m_Size-1)
	return false;
m_Position=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

SIZE_T Buffer::Fill(UINT value, SIZE_T size)
{
if(m_Size==0)
	return 0;
SIZE_T available=m_Size-m_Position;
if(size==0)
	size=available;
SIZE_T copy=TypeHelper::Min(size, available);
MemoryHelper::Fill(&m_Buffer[m_Position], copy, value);
m_Position+=copy;
return copy;
}


//==========================
// Con-/Destructors Private
//==========================

Buffer::Buffer(SIZE_T size):
m_Buffer(new BYTE[size]),
m_Options(BufferOptions::None),
m_Position(0),
m_Size(size)
{}

Buffer::Buffer(VOID const* buf, SIZE_T size, BufferOptions options):
m_Buffer(nullptr),
m_Options(options),
m_Position(0),
m_Size(size)
{
switch(options)
	{
	case BufferOptions::Move:
		{
		if(size==0)
			throw InvalidArgumentException();
		m_Buffer=(BYTE*)buf;
		break;
		}
	case BufferOptions::Static:
		{
		m_Buffer=(BYTE*)buf;
		break;
		}
	default:
		{
		if(size==0)
			throw InvalidArgumentException();
		m_Buffer=new BYTE[size];
		MemoryHelper::Copy(m_Buffer, buf, size);
		break;
		}
	}
}

}