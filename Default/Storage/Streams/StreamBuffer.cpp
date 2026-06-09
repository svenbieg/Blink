//==================
// StreamBuffer.cpp
//==================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#include "StreamBuffer.h"


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

StreamBuffer::~StreamBuffer()noexcept
{
FreeChunks(m_First);
FreeChunks(m_Free);
}


//========
// Common
//========

VOID StreamBuffer::Clear()noexcept
{
SpinLock lock(m_CriticalSection);
auto next=m_First->Next;
auto free=m_Free;
m_First->Next=nullptr;
m_Free=nullptr;
m_Last=m_First;
m_Read=0;
m_Size=0;
m_Written=0;
lock.Unlock();
FreeChunks(next);
FreeChunks(free);
}


//==============
// Input-Stream
//==============

SIZE_T StreamBuffer::Available()
{
SpinLock lock(m_CriticalSection);
return m_Written-m_Read;
}

SIZE_T StreamBuffer::Read(VOID* buf, SIZE_T size)
{
SpinLock lock(m_CriticalSection);
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	SIZE_T available=m_Written-m_Read;
	if(!available)
		{
		m_Signal.Wait(lock);
		continue;
		}
	BYTE const* src=m_First->Buffer;
	SIZE_T buf_pos=m_Read;
	SIZE_T buf_size=m_First->Size;
	lock.Unlock();
	SIZE_T copy=TypeHelper::Min(buf_size-buf_pos, size-pos);
	MemoryHelper::Copy(&dst[pos], &src[buf_pos], copy);
	pos+=copy;
	lock.Lock();
	m_Read+=copy;
	if(m_Read==m_Size)
		{
		m_First->Size=0;
		m_Read=0;
		m_Size=0;
		m_Written=0;
		continue;
		}
	if(m_Read==m_ChunkSize)
		{
		auto free=m_First;
		m_First=m_First->Next;
		assert(m_First);
		free->Next=m_Free;
		m_Free=free;
		m_Read=0;
		m_Size-=m_ChunkSize;
		m_Written-=m_ChunkSize;
		}
	}
return pos;
}


//===============
// Output-Stream
//===============

VOID StreamBuffer::Flush()
{
SpinLock lock(m_CriticalSection);
m_Written=m_Size;
m_Signal.Trigger();
}

SIZE_T StreamBuffer::Write(VOID const* buf, SIZE_T size)
{
SpinLock lock(m_CriticalSection);
auto src=(BYTE const*)buf;
SIZE_T pos=0;
auto current=m_Last;
while(pos<size)
	{
	SIZE_T available=0;
	available=m_ChunkSize-current->Size;
	if(!available)
		{
		lock.Unlock();
		current=CreateChunk();
		lock.Lock();
		m_Last->Next=current;
		m_Last=current;
		available=m_ChunkSize;
		}
	auto buf=current->Buffer;
	auto buf_pos=current->Size;
	lock.Unlock();
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	MemoryHelper::Copy(&buf[buf_pos], &src[pos], copy);
	pos+=copy;
	lock.Lock();
	current->Size+=copy;
	m_Size+=copy;
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

StreamBuffer::StreamBuffer(SIZE_T chunk_size):
m_ChunkSize(chunk_size),
m_First(nullptr),
m_Free(nullptr),
m_Last(nullptr),
m_Read(0),
m_Size(0),
m_Written(0)
{
m_First=CreateChunk();
m_Last=m_First;
}


//================
// Common Private
//================

StreamBuffer::CHUNK* StreamBuffer::CreateChunk()
{
CHUNK* chunk=nullptr;
if(m_Free)
	{
	chunk=m_Free;
	m_Free=m_Free->Next;
	}
else
	{
	chunk=(CHUNK*)MemoryHelper::Allocate(sizeof(CHUNK)+m_ChunkSize);
	}
chunk->Next=nullptr;
chunk->Size=0;
return chunk;
}

VOID StreamBuffer::FreeChunks(CHUNK* chunk)
{
while(chunk)
	{
	auto next=chunk->Next;
	MemoryHelper::Free(chunk);
	chunk=next;
	}
}

}}