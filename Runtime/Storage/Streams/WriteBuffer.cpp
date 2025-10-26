//==================
// WriteBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/WriteBuffer.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

WriteBuffer::~WriteBuffer()
{
FreeBlocks(m_First);
FreeBlocks(m_Free);
}


//========
// Common
//========

VOID WriteBuffer::Clear()
{
SpinLock lock(m_CriticalSection);
auto first_next=m_First->Next;
auto free=m_Free;
m_First->Next=nullptr;
m_Free=nullptr;
m_Last=m_First;
m_Read=0;
m_Size=0;
m_Written=0;
lock.Unlock();
FreeBlocks(first_next);
FreeBlocks(free);
}

VOID WriteBuffer::Flush()
{
SpinLock lock(m_CriticalSection);
m_Written=m_Size;
}

SIZE_T WriteBuffer::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T pos=0;
SpinLock lock(m_CriticalSection);
while(pos<size)
	{
	SIZE_T available=m_Written-m_Read;
	if(!available)
		break;
	auto src=m_First->Buffer;
	SIZE_T buf_pos=m_Read;
	SIZE_T buf_size=m_First->Size;
	lock.Unlock();
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	copy=TypeHelper::Min(copy, buf_size);
	MemoryHelper::Copy(&dst[pos], &src[buf_pos], copy);
	pos+=copy;
	lock.Lock();
	m_Read+=copy;
	if(m_Read==buf_size)
		{
		auto next=m_First->Next;
		if(next)
			{
			auto first=m_First;
			m_First=next;
			m_Read=0;
			m_Size-=buf_size;
			m_Written-=buf_size;
			lock.Unlock();
			FreeBlock(first);
			lock.Lock();
			}
		else if(m_Read==m_Size)
			{
			m_First->Size=0;
			m_Read=0;
			m_Size=0;
			m_Written=0;
			}
		}
	}
return pos;
}


SIZE_T WriteBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T written=0;
auto src=(BYTE const*)buf;
SIZE_T pos=0;
SpinLock lock(m_CriticalSection);
auto current=m_Last;
while(pos<size)
	{
	auto dst=current->Buffer;
	SIZE_T buf_size=current->Size;
	lock.Unlock();
	SIZE_T available=m_BlockSize-buf_size;
	if(!available)
		{
		auto next=CreateBlock();
		lock.Lock();
		current->Next=next;
		m_Last=current;
		lock.Unlock();
		current=next;
		buf_size=0;
		available=m_BlockSize;
		}
	SIZE_T write=size-pos;
	SIZE_T copy=TypeHelper::Min(available, write);
	MemoryHelper::Copy(&dst[buf_size], &src[pos], copy);
	pos+=copy;
	lock.Lock();
	current->Size+=copy;
	m_Size+=copy;
	}
return written;
}


//==========================
// Con-/Destructors Private
//==========================

WriteBuffer::WriteBuffer(UINT block_size):
m_BlockSize(block_size),
m_First(nullptr),
m_Free(nullptr),
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

WriteBuffer::WriteBufferBlock* WriteBuffer::CreateBlock()
{
WriteBufferBlock* block=nullptr;
if(m_Free)
	{
	block=m_Free;
	m_Free=m_Free->Next;
	}
else
	{
	block=(WriteBufferBlock*)operator new(sizeof(WriteBufferBlock)+m_BlockSize);
	}
block->Next=nullptr;
block->Size=0;
return block;
}

VOID WriteBuffer::FreeBlock(WriteBufferBlock* block)
{
block->Next=m_Free;
m_Free=block;
}

VOID WriteBuffer::FreeBlocks(WriteBufferBlock* first)
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