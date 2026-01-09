//==================
// OutputBuffer.cpp
//==================

#include "OutputBuffer.h"


//=======
// Using
//=======

#include "Concurrency/SpinLock.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

OutputBuffer::~OutputBuffer()
{
FreeBlocks(m_First);
FreeBlocks(m_Free);
}


//========
// Common
//========

VOID OutputBuffer::Clear()
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

VOID OutputBuffer::Flush()
{
SpinLock lock(m_CriticalSection);
m_Written=m_Size;
}

SIZE_T OutputBuffer::Read(VOID* buf, SIZE_T size)
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


SIZE_T OutputBuffer::Write(VOID const* buf, SIZE_T size)
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

OutputBuffer::OutputBuffer(UINT block_size):
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

OutputBuffer::OutputBufferBlock* OutputBuffer::CreateBlock()
{
OutputBufferBlock* block=nullptr;
if(m_Free)
	{
	block=m_Free;
	m_Free=m_Free->Next;
	}
else
	{
	block=(OutputBufferBlock*)operator new(sizeof(OutputBufferBlock)+m_BlockSize);
	}
block->Next=nullptr;
block->Size=0;
return block;
}

VOID OutputBuffer::FreeBlock(OutputBufferBlock* block)
{
block->Next=m_Free;
m_Free=block;
}

VOID OutputBuffer::FreeBlocks(OutputBufferBlock* first)
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