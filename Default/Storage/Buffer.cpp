//============
// Buffer.cpp
//============

#include "Buffer.h"


//=======
// Using
//=======

#include <new>
#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Handle<Buffer> Buffer::Create(SIZE_T size)
{
UINT buf_size=sizeof(Buffer)+size;
auto buf=(Buffer*)MemoryHelper::Allocate(buf_size);
auto buf_ptr=(BYTE*)((SIZE_T)buf+sizeof(Buffer));
new (buf) Buffer(buf_ptr, size);
return buf;
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

Buffer::Buffer(BYTE* buf_ptr, SIZE_T size):
m_Buffer(buf_ptr),
m_Position(0),
m_Size(size)
{}

}