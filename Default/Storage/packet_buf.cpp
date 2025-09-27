//================
// packet_buf.cpp
//================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/packet_buf.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

packet_buf::packet_buf(UINT block_size):
m_block_size(block_size),
m_first(nullptr),
m_last(nullptr),
m_read(0),
m_write(0),
m_written(0)
{}

packet_buf::~packet_buf()
{
clear();
}


//========
// Common
//========

VOID packet_buf::clear()
{
auto block=m_first;
while(block)
	{
	auto next=block->next;
	operator delete(block);
	block=next;
	}
m_first=nullptr;
m_last=nullptr;
m_read=0;
m_write=0;
m_written=0;
}

SIZE_T packet_buf::read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
size_t pos=0;
while(pos<size)
	{
	if(!m_first)
		break;
	size_t block_size=m_first->size;
	UINT available=block_size-m_read;
	if(available)
		{
		UINT copy=TypeHelper::Min(available, size-pos);
		MemoryHelper::Copy(&dst[pos], &m_first->buf[m_read], copy);
		m_read+=copy;
		pos+=copy;
		}
	if(m_read==block_size)
		{
		auto next=m_first->next;
		operator delete(m_first);
		m_first=next;
		if(!m_first)
			m_last=nullptr;
		m_read-=block_size;
		m_write-=block_size;
		m_written-=block_size;
		}
	}
return pos;
}

SIZE_T packet_buf::write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE const*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	if(!m_last||m_last->size==m_block_size)
		{
		auto block=(block_t*)operator new(sizeof(block_t)+m_block_size);
		block->next=nullptr;
		block->size=0;
		if(m_last)
			m_last->next=block;
		m_last=block;
		if(!m_first)
			m_first=m_last;
		}
	UINT available=m_block_size-m_last->size;
	UINT copy=TypeHelper::Min(available, size-pos);
	MemoryHelper::Copy(&m_last->buf[m_last->size], &src[pos], copy);
	m_last->size+=copy;
	m_write+=copy;
	pos+=copy;
	}
return pos;
}

}