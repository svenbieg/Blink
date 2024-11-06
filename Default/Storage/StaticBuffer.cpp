//==================
// StaticBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "StaticBuffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

StaticBuffer::StaticBuffer(VOID const* buf, SIZE_T size):
m_Buffer((BYTE*)buf),
m_Position(0),
m_Size(size)
{}


//==============
// Input-Stream
//==============

SIZE_T StaticBuffer::Available()
{
if(m_Size>m_Position)
	return m_Size-m_Position;
return 0;
}

SIZE_T StaticBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=0;
if(m_Size>m_Position)
	available=m_Size-m_Position;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(buf)
	CopyMemory(buf, &m_Buffer[m_Position], copy);
m_Position+=copy;
return copy;
}


//===============
// Output-Stream
//===============

SIZE_T StaticBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T available=0;
if(m_Size>m_Position)
	available=m_Size-m_Position;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
CopyMemory(&m_Buffer[m_Position], buf, copy);
m_Position+=copy;
return copy;
}


//==========
// Seekable
//==========

BOOL StaticBuffer::Seek(FILE_SIZE pos)
{
if(m_Size>0)
	{
	if(pos>=m_Size)
		return false;
	}
m_Position=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

SIZE_T StaticBuffer::Fill(UINT value, SIZE_T size)
{
SIZE_T available=0;
if(m_Size>m_Position)
	available=m_Size-m_Position;
if(size==0)
	size=available;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(!copy)
	return 0;
FillMemory(&m_Buffer[m_Position], copy, value);
m_Position+=copy;
return copy;
}

SIZE_T StaticBuffer::Zero(SIZE_T size)
{
SIZE_T available=0;
if(m_Size>m_Position)
	available=m_Size-m_Position;
if(size==0)
	size=available;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(!copy)
	return 0;
ZeroMemory(&m_Buffer[m_Position], copy);
m_Position+=copy;
return copy;
}

}