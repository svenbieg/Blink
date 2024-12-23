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

Buffer::Buffer(SIZE_T size):
m_Buffer(new BYTE[size]),
m_Position(0),
m_Size(size)
{}

Buffer::Buffer(VOID const* buf, SIZE_T size, BufferOptions options):
m_Buffer(nullptr),
m_Options(options),
m_Position(0),
m_Size(size)
{
assert(size!=0);
switch(options)
	{
	case BufferOptions::Static:
		{
		m_Buffer=(BYTE*)buf;
		break;
		}
	default:
		{
		m_Buffer=new BYTE[size];
		MemoryHelper::Copy(m_Buffer, buf, size);
		break;
		}
	}
}

Buffer::~Buffer()
{
if(m_Options!=BufferOptions::Static)
	delete m_Buffer;
}


//==============
// Input-Stream
//==============

SIZE_T Buffer::Available()
{
return m_Size-m_Position;
}

SIZE_T Buffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=m_Size-m_Position;
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
if(pos>m_Size-1)
	return false;
m_Position=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

SIZE_T Buffer::Fill(UINT value, SIZE_T size)
{
SIZE_T available=m_Size-m_Position;
if(size==0)
	size=available;
SIZE_T copy=TypeHelper::Min(size, available);
MemoryHelper::Fill(&m_Buffer[m_Position], copy, value);
m_Position+=copy;
return copy;
}

}