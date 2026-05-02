//==================
// StreamBuffer.cpp
//==================

#include "StreamBuffer.h"


//=======
// Using
//=======

#include "Concurrency/ReadLock.h"
#include "Concurrency/WriteLock.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==================
// Con-/Destructors
//==================

StreamBuffer::~StreamBuffer()
{
FreeChunks(m_First);
}


//========
// Common
//========

VOID StreamBuffer::Clear()
{
WriteLock lock(m_Mutex);
auto chunk=m_First;
m_First=nullptr;
m_Last=nullptr;
m_Read=0;
m_Size=0;
m_Written=0;
lock.Unlock();
FreeChunks(chunk);
}


//==============
// Input-Stream
//==============

SIZE_T StreamBuffer::Available()
{
ReadLock lock(m_Mutex);
return m_Written-m_Read;
}

SIZE_T StreamBuffer::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T pos=0;
WriteLock lock(m_Mutex);
while(pos<size)
	{
	SIZE_T available=TypeHelper::Min(m_Written-m_Read, m_ChunkSize);
	if(!available)
		{
		m_Signal.Wait(lock);
		continue;
		}
	BYTE const* src=m_First->Buffer;
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	MemoryHelper::Copy(&dst[pos], &src[m_Read], copy);
	pos+=copy;
	m_Read+=copy;
	if(m_Read==m_Size)
		{
		m_Read=0;
		m_Size=0;
		m_Written=0;
		continue;
		}
	if(m_Read==m_ChunkSize)
		{
		FreeChunk();
		m_Read-=m_ChunkSize;
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
WriteLock lock(m_Mutex);
m_Written=m_Size;
m_Signal.Trigger();
}

SIZE_T StreamBuffer::Write(VOID const* buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
auto src=(BYTE const*)buf;
SIZE_T pos=0;
auto chunk=m_Last;
while(pos<size)
	{
	SIZE_T available=0;
	if(chunk)
		available=m_ChunkSize-chunk->Size;
	if(!available)
		{
		auto next=CreateChunk();
		chunk->Next=next;
		chunk=next;
		available=m_ChunkSize;
		}
	auto dst=chunk->Buffer;
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	MemoryHelper::Copy(&dst[chunk->Size], &src[pos], copy);
	pos+=copy;
	chunk->Size+=copy;
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
m_Last(nullptr),
m_Read(0),
m_Size(0),
m_Written(0)
{}


//================
// Common Private
//================

StreamBuffer::CHUNK* StreamBuffer::CreateChunk()
{
CHUNK* chunk=(CHUNK*)MemoryHelper::Allocate(sizeof(CHUNK)+m_ChunkSize);
chunk->Next=nullptr;
chunk->Size=0;
if(!m_First)
	m_First=chunk;
m_Last=chunk;
return chunk;
}

VOID StreamBuffer::FreeChunk()
{
auto chunk=m_First;
m_First=chunk->Next;
if(!m_First)
	m_Last=nullptr;
MemoryHelper::Free(chunk);
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