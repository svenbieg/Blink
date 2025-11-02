//================
// ReadBuffer.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/ReadBuffer.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

ReadBuffer::~ReadBuffer()
{
FreeBlocks(m_First);
}


//========
// Common
//========

SIZE_T ReadBuffer::Available()
{
ReadLock lock(m_Mutex);
return m_Written-m_Read;
}

VOID ReadBuffer::Clear()
{
WriteLock lock(m_Mutex);
FreeBlocks(m_First->Next);
m_First->Next=nullptr;
m_Last=m_First;
m_Read=0;
m_Size=0;
m_Written=0;
}

VOID ReadBuffer::Flush()
{
WriteLock lock(m_Mutex);
m_Written=m_Size;
m_Signal.Trigger();
}

SIZE_T ReadBuffer::Read(VOID* buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	auto src=m_First->Buffer;
	SIZE_T available=m_Written-m_Read;
	if(!available)
		{
		m_Signal.Wait(lock);
		continue;
		}
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	SIZE_T buf_size=m_First->Size;
	copy=TypeHelper::Min(copy, buf_size-m_Read);
	MemoryHelper::Copy(&dst[pos], &src[m_Read], copy);
	m_Read+=copy;
	pos+=copy;
	if(m_Read==buf_size)
		{
		auto next=m_First->Next;
		if(next)
			{
			auto first=m_First;
			m_First=next;
			operator delete(first);
			m_Read=0;
			m_Size-=buf_size;
			m_Written-=buf_size;
			}
		else if(m_Read==m_Size)
			{
			m_Read=0;
			m_Size=0;
			m_Written=0;
			}
		}
	}
return pos;
}

SIZE_T ReadBuffer::Write(RingBuffer* ring_buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
SIZE_T written=0;
auto current=m_Last;
BYTE* src=nullptr;
SIZE_T ring_size=ring_buf->BeginRead(&src);
while(ring_size)
	{
	SIZE_T pos=0;
	while(pos<ring_size)
		{
		auto dst=current->Buffer;
		SIZE_T buf_size=current->Size;
		SIZE_T available=m_BlockSize-buf_size;
		if(!available)
			{
			auto next=CreateBlock();
			current->Next=next;
			current=next;
			m_Last=current;
			buf_size=0;
			available=m_BlockSize;
			}
		SIZE_T write=size-pos;
		SIZE_T copy=TypeHelper::Min(available, write);
		MemoryHelper::Copy(&dst[buf_size], &src[pos], copy);
		current->Size+=copy;
		pos+=copy;
		m_Size+=copy;
		}
	ring_buf->Consumed(written);
	ring_size=ring_buf->BeginRead(&src);
	}
return written;
}


//==========================
// Con-/Destructors Private
//==========================

ReadBuffer::ReadBuffer(UINT block_size):
m_BlockSize(block_size),
m_First(nullptr),
m_Last(nullptr),
m_Read(0),
m_Size(0),
m_Written(0)
{
m_First=CreateBlock();
m_Last=m_First;
}


//================
// Common Private
//================

ReadBuffer::ReadBufferBlock* ReadBuffer::CreateBlock()
{
ReadBufferBlock* block=(ReadBufferBlock*)operator new(sizeof(ReadBufferBlock)+m_BlockSize);
block->Next=nullptr;
block->Size=0;
return block;
}

VOID ReadBuffer::FreeBlocks(ReadBufferBlock* first)
{
auto buf=first;
while(buf)
	{
	auto next=buf->Next;
	operator delete(buf);
	buf=next;
	}
}

}}