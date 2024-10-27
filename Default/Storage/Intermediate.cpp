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

Intermediate::Intermediate():
m_First(nullptr),
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
auto buf=m_First;
while(buf)
	{
	auto next=buf->Next;
	delete buf;
	buf=next;
	}
m_First=nullptr;
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
auto buf=m_First->Next;
while(buf)
	{
	available+=buf->Size;
	buf=buf->Next;
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
	if(!m_First)
		m_Written.Wait(lock);
	if(!m_First)
		break;
	SIZE_T available=m_First->Size-m_Offset;
	SIZE_T copy=Min(size-pos, available);
	CopyMemory(&dst[pos], &m_First->Buffer[m_Offset], copy);
	m_Offset+=copy;
	if(m_Offset==m_First->Size)
		{
		auto next=m_First->Next;
		delete m_First;
		m_First=next;
		if(!m_First)
			m_Last=nullptr;
		m_Offset=0;
		}
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
	m_First=new BUFFER();
	m_First->Size=0;
	m_First->Next=nullptr;
	m_Last=m_First;
	}
auto current=m_Last;
auto src=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	if(current->Size==PAGE_SIZE)
		{
		auto next=new BUFFER();
		next->Size=0;
		next->Next=nullptr;
		current->Next=next;
		m_Last=next;
		current=next;
		}
	SIZE_T available=PAGE_SIZE-current->Size;
	SIZE_T copy=Min(size-pos, available);
	CopyMemory(&current->Buffer[current->Size], &src[pos], copy);
	current->Size+=copy;
	pos+=copy;
	}
return pos;
}

}
