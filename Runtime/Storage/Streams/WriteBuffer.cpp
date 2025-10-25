//==================
// WriteBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/Streams/WriteBuffer.h"


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
FreeBlocks(m_First->Next);
m_First->Next=nullptr;
m_Last=m_First;
FreeBlocks(m_Free);
m_Free=nullptr;
m_Read=0;
m_Size=0;
m_Written=0;
}

VOID WriteBuffer::Flush()
{
m_Written=m_Size;
}

SIZE_T WriteBuffer::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	SIZE_T available=m_Written-m_Read;
	if(!available)
		break;
	auto src=m_First->Buffer;
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	SIZE_T buf_size=m_First->Size;
	copy=TypeHelper::Min(copy, buf_size);
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
			FreeBlock(first);
			m_Read=0;
			m_Size-=buf_size;
			m_Written-=buf_size;
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
auto current=m_Last;
while(pos<size)
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