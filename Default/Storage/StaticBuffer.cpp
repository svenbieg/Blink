//==================
// StaticBuffer.cpp
//==================

#include "StaticBuffer.h"


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

SIZE_T StaticBuffer::Available()
{
return m_Size-m_Position;
}

SIZE_T StaticBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Position);
MemoryHelper::Copy(buf, &m_Buffer[m_Position], copy);
m_Position+=copy;
return copy;
}


//===============
// Output-Stream
//===============

VOID StaticBuffer::Flush()
{
}

SIZE_T StaticBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Position);
MemoryHelper::Copy(&m_Buffer[m_Position], buf, copy);
m_Position+=copy;
return copy;
}


//==========
// Seekable
//==========

SIZE_T StaticBuffer::GetSize()
{
return m_Size;
}

BOOL StaticBuffer::Seek(SIZE_T pos)
{
if(pos>=m_Size)
	return false;
m_Position=(SIZE_T)pos;
return true;
}


//==========================
// Con-/Destructors Private
//==========================

StaticBuffer::StaticBuffer(VOID* buf, SIZE_T size):
m_Buffer((BYTE*)buf),
m_Position(0),
m_Size(size)
{}

}