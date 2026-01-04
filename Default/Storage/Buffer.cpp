//============
// Buffer.cpp
//============

#include "Buffer.h"


//=======
// Using
//=======

#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {


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

VOID Buffer::Flush()
{
}

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

SIZE_T Buffer::GetSize()
{
return m_Size;
}

BOOL Buffer::Seek(SIZE_T pos)
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

Buffer::Buffer(VOID* buf_ptr, SIZE_T size):
m_Buffer((BYTE*)buf_ptr),
m_Position(0),
m_Size(size)
{}

}