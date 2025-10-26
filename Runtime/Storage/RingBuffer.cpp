//================
// RingBuffer.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include <new>
#include "Storage/RingBuffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Handle<RingBuffer> RingBuffer::Create(SIZE_T capacity)
{
auto ring_buf=(RingBuffer*)operator new(sizeof(RingBuffer)+capacity);
auto buf=(BYTE*)((SIZE_T)ring_buf+sizeof(RingBuffer));
new (ring_buf) RingBuffer(buf, capacity);
return ring_buf;
}


//========
// Common
//========

SIZE_T RingBuffer::Available()
{
return m_Head-m_Tail;
}

SIZE_T RingBuffer::AvailableForWrite()
{
return m_Size-(m_Head-m_Tail);
}

SIZE_T RingBuffer::BeginRead(BYTE** buf_ptr)
{
SIZE_T available=m_Head-m_Tail;
SIZE_T tail=m_Tail%m_Size;
SIZE_T copy=TypeHelper::Min(available, m_Size-tail);
*buf_ptr=&m_Buffer[tail];
return copy;
}

SIZE_T RingBuffer::BeginWrite(BYTE** buf_ptr)
{
SIZE_T available=m_Size-(m_Head-m_Tail);
SIZE_T head=m_Head%m_Size;
SIZE_T copy=TypeHelper::Min(available, m_Size-head);
*buf_ptr=&m_Buffer[head];
return copy;
}

VOID RingBuffer::Consume(SIZE_T size)
{
m_Tail+=size;
if(m_Head==m_Tail)
	{
	m_Head=0;
	m_Tail=0;
	}
}

SIZE_T RingBuffer::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T available=m_Head-m_Tail;
SIZE_T read=TypeHelper::Min(size, available);
SIZE_T pos=0;
while(pos<size)
	{
	if(m_Head==m_Tail)
		{
		m_Head=0;
		m_Tail=0;
		break;
		}
	SIZE_T tail=m_Tail%m_Size;
	SIZE_T copy=TypeHelper::Min(read, m_Size-tail);
	MemoryHelper::Copy(&dst[pos], &m_Buffer[tail], copy);
	m_Tail+=copy;
	pos+=copy;
	}
return pos;
}

SIZE_T RingBuffer::Write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE const*)buf;
SIZE_T available=m_Size-(m_Head-m_Tail);
SIZE_T write=TypeHelper::Min(available, size);
SIZE_T pos=0;
while(pos<write)
	{
	SIZE_T head=m_Head%m_Size;
	SIZE_T copy=TypeHelper::Min(write-pos, m_Size-head);
	MemoryHelper::Copy(&m_Buffer[head], &src[pos], copy);
	m_Head+=copy;
	pos+=copy;
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

RingBuffer::RingBuffer(BYTE* buf, SIZE_T size):
m_Buffer(buf),
m_Head(0),
m_Size(size),
m_Tail(0)
{}

}