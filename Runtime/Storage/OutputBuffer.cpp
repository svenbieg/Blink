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


//==================
// Con-/Destructors
//==================

OutputBuffer::~OutputBuffer()noexcept
{
FreeBlocks(m_First);
FreeBlocks(m_Free);
}


//========
// Common
//========

SIZE_T OutputBuffer::Available()noexcept
{
SpinLock lock(m_CriticalSection);
return m_Written-m_Read;
}

VOID OutputBuffer::Clear()noexcept
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

VOID OutputBuffer::Flush()noexcept
{
SpinLock lock(m_CriticalSection);
m_Written=m_Size;
}

SIZE_T OutputBuffer::Read(VOID* buf, SIZE_T size)noexcept
{
auto dst=(BYTE*)buf;
SIZE_T read=0;
SpinLock lock(m_CriticalSection);
while(read<size)
	{
	SIZE_T available=m_Written-m_Read;
	if(!available)
		break;
	auto src=m_First->Buffer;
	SIZE_T buf_pos=m_Read;
	SIZE_T buf_size=m_First->Size;
	lock.Unlock();
	SIZE_T copy=TypeHelper::Min(available, size-read);
	copy=TypeHelper::Min(copy, buf_size);
	MemoryHelper::Copy(&dst[read], &src[buf_pos], copy);
	read+=copy;
	m_Read+=copy;
	lock.Lock();
	if(m_Read==m_Written)
		{
		m_First->Size=0;
		m_Read=0;
		m_Size=0;
		m_Written=0;
		break;
		}
	if(m_Read==m_BlockSize)
		{
		auto first=m_First;
		auto next=first->Next;
		m_First=next;
		m_Read=0;
		m_Size-=m_BlockSize;
		m_Written-=m_BlockSize;
		FreeBlock(first);
		}
	}
return read;
}

SIZE_T OutputBuffer::Write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE const*)buf;
SIZE_T written=0;
SpinLock lock(m_CriticalSection);
auto current=m_Last;
while(written<size)
	{
	auto dst=current->Buffer;
	SIZE_T buf_size=current->Size;
	lock.Unlock();
	SIZE_T available=m_BlockSize-buf_size;
	SIZE_T copy=0;
	if(available)
		{
		copy=TypeHelper::Min(available, size-written);
		MemoryHelper::Copy(&dst[buf_size], &src[written], copy);
		written+=copy;
		m_Size+=copy;
		lock.Lock();
		current->Size+=copy;
		}
	else
		{
		auto next=CreateBlock();
		copy=TypeHelper::Min(m_BlockSize, size-written);
		MemoryHelper::Copy(next->Buffer, &src[written], copy);
		written+=copy;
		m_Size+=copy;
		lock.Lock();
		current->Next=next;
		current=next;
		current->Size+=copy;
		m_Last=current;
		}
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
	block=(OutputBufferBlock*)MemoryHelper::Allocate(sizeof(OutputBufferBlock)+m_BlockSize);
	}
block->Next=nullptr;
block->Size=0;
return block;
}

VOID OutputBuffer::FreeBlock(OutputBufferBlock* block)noexcept
{
block->Next=m_Free;
m_Free=block;
}

VOID OutputBuffer::FreeBlocks(OutputBufferBlock* first)noexcept
{
auto buf=first;
while(buf)
	{
	auto next=buf->Next;
	MemoryHelper::Free(buf);
	buf=next;
	}
}

}