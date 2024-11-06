//==================
// Intermediate.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Intermediate.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Intermediate::Intermediate(UINT block_size):
m_BlockSize(block_size),
m_First(nullptr),
m_Free(nullptr),
m_Last(nullptr),
m_Offset(0)
{}

Intermediate::~Intermediate()
{
Clear();
}


//========
// Common
//========

VOID Intermediate::Clear()
{
FreeBlocks(m_First);
FreeBlocks(m_Free);
m_First=nullptr;
m_Free=nullptr;
m_Last=nullptr;
m_Offset=0;
}


//==============
// Input-Stream
//==============

SIZE_T Intermediate::Available()
{
ScopedLock lock(m_Mutex);
if(!m_First)
	return 0;
SIZE_T available=m_First->Size-m_Offset;
auto block=m_First->Next;
while(block)
	{
	available+=block->Size;
	block=block->Next;
	}
return available;
}

SIZE_T Intermediate::Read(VOID* buf, SIZE_T size)
{
ScopedLock lock(m_Mutex);
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	if(m_Offset==m_BlockSize)
		{
		auto next=m_First->Next;
		CacheBlock(m_First);
		m_First=next;
		if(!next)
			m_Last=nullptr;
		m_Offset=0;
		}
	SIZE_T available=0;
	if(m_First)
		available=m_First->Size-m_Offset;
	if(!available)
		{
		m_Written.Wait(lock);
		continue;
		}
	SIZE_T copy=Min(size-pos, available);
	CopyMemory(&dst[pos], &m_First->Buffer[m_Offset], copy);
	m_Offset+=copy;
	pos+=copy;
	}
return pos;
}


//===============
// Output-Stream
//===============

VOID Intermediate::Flush()
{
m_Written.Trigger();
}

SIZE_T Intermediate::Write(VOID const* buf, SIZE_T size)
{
ScopedLock lock(m_Mutex);
if(!m_First)
	{
	m_First=(BLOCK*)AllocateBlock();
	m_Last=m_First;
	}
auto current=m_Last;
auto src=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	if(current->Size==m_BlockSize)
		{
		auto next=(BLOCK*)AllocateBlock();
		current->Next=next;
		current=next;
		m_Last=current;
		}
	SIZE_T available=m_BlockSize-current->Size;
	SIZE_T copy=Min(size-pos, available);
	CopyMemory(&current->Buffer[current->Size], &src[pos], copy);
	current->Size+=copy;
	pos+=copy;
	}
return pos;
}


//================
// Common Private
//================

VOID* Intermediate::AllocateBlock()
{
BLOCK* block=nullptr;
if(m_Free)
	{
	block=m_Free;
	m_Free=m_Free->Next;
	}
else
	{
	block=(BLOCK*)operator new(sizeof(BLOCK)+m_BlockSize);
	}
block->Next=nullptr;
block->Size=0;
return block;
}

VOID Intermediate::CacheBlock(BLOCK* block)
{
block->Next=m_Free;
m_Free=block;
}

VOID Intermediate::FreeBlocks(BLOCK* block)
{
while(block)
	{
	auto next=block->Next;
	delete block;
	block=next;
	}
}

}
