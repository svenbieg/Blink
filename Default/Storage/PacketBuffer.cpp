//==================
// PacketBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include <new>
#include "Storage/PacketBuffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Handle<PacketBuffer> PacketBuffer::Create(SIZE_T size)
{
auto pbuf=(PacketBuffer*)operator new(sizeof(PacketBuffer)+size);
auto buf=(BYTE*)pbuf+sizeof(PacketBuffer);
new (pbuf) PacketBuffer(buf, size);
return pbuf;
}


//========
// Common
//========

Handle<PacketBuffer> PacketBuffer::Begin()
{
auto pbuf=this;
while(pbuf)
	{
	if(pbuf->Available())
		break;
	pbuf=pbuf->Next();
	}
return pbuf;
}

Handle<PacketBuffer> PacketBuffer::End()
{
auto pbuf=this;
while(pbuf->m_Next)
	{
	pbuf=pbuf->m_Next;
	}
return pbuf;
}

SIZE_T PacketBuffer::Read(Handle<PacketBuffer>* pbuf_ptr, VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T read=0;
SIZE_T remain=size;
while(remain)
	{
	auto pbuf=*pbuf_ptr;
	if(pbuf==nullptr)
		break;
	SIZE_T available=pbuf->Available();
	SIZE_T copy=TypeHelper::Min(remain, available);
	read+=pbuf->Read(&dst[read], copy);
	remain-=copy;
	auto next=pbuf->m_Next;
	if(pbuf->Available()==0)
		*pbuf_ptr=next;
	pbuf=next;
	}
return read;
}

SIZE_T PacketBuffer::TotalAvailable(PacketBuffer* pbuf)
{
SIZE_T available=0;
while(pbuf)
	{
	available+=pbuf->m_Written-pbuf->m_Read;
	pbuf=pbuf->m_Next;
	}
return available;
}

SIZE_T PacketBuffer::Write(Handle<PacketBuffer>* pbuf_ptr, VOID const* buf, SIZE_T size, SIZE_T block_size)
{
if(*pbuf_ptr==nullptr)
	*pbuf_ptr=PacketBuffer::Create(block_size);
auto pbuf=(*pbuf_ptr)->End();
SIZE_T empty=pbuf->Empty();
SIZE_T copy=TypeHelper::Min(size, empty);
SIZE_T written=0;
if(copy)
	{
	written+=pbuf->Write(buf, copy);
	if(written==size)
		return written;
	}
auto src=(BYTE const*)buf;
SIZE_T remain=size-copy;
SIZE_T pbuf_size=TypeHelper::AlignUp(remain, PAGE_SIZE);
auto append=PacketBuffer::Create(pbuf_size);
append->Write(&src[written], remain);
pbuf->Append(append);
return size;
}


//==============
// Input-Stream
//==============

SIZE_T PacketBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=m_Written-m_Read;
SIZE_T copy=TypeHelper::Min(size, available);
MemoryHelper::Copy(buf, &m_Buffer[m_Read], copy);
m_Read+=copy;
return copy;
}


//===============
// Output-Stream
//===============

VOID PacketBuffer::Flush()
{
}

SIZE_T PacketBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T available=m_Size-m_Written;
SIZE_T copy=TypeHelper::Min(size, available);
MemoryHelper::Copy(&m_Buffer[m_Written], buf, copy);
m_Written+=copy;
return copy;
}


//==========================
// Con-/Destructors Private
//==========================

PacketBuffer::PacketBuffer(BYTE* buf, SIZE_T size):
m_Buffer(buf),
m_Read(0),
m_Size(size),
m_Written(0)
{}

}