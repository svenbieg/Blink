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
uReadPos(0),
uWritePos(0)
{}

Intermediate::~Intermediate()
{
for(auto it=cBuffers.cbegin(); it.has_current(); it.move_next())
	{
	auto buf=it.get_current();
	delete buf;
	}
}


//==============
// Input-Stream
//==============

SIZE_T Intermediate::Available()
{
ScopedLock lock(cMutex);
return uWritePos-uReadPos;
}

SIZE_T Intermediate::Read(VOID* buf, SIZE_T size)
{
ScopedLock lock(cMutex);
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	if(uReadPos==uWritePos)
		cWritten.Wait(lock);
	SIZE_T available=uWritePos-uReadPos;
	if(!available)
		break;
	SIZE_T copy=Min(size-pos, available);
	BYTE* src=nullptr;
	UINT src_pos=0;
	UINT block=GetBuffer(uReadPos, &src, &src_pos);
	copy=Min(copy, (SIZE_T)block);
	if(dst)
		CopyMemory(&dst[pos], &src[src_pos], copy);
	uReadPos+=copy;
	DiscardBuffers();
	pos+=copy;
	}
return pos;
}


//===============
// Output-Stream
//===============

SIZE_T Intermediate::Write(VOID const* buf, SIZE_T size)
{
ScopedLock lock(cMutex);
auto src=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	BYTE* dst=nullptr;
	UINT dst_pos=0;
	UINT block=GetBuffer(uWritePos, &dst, &dst_pos);
	if(!block)
		block=AllocBuffer(&dst, &dst_pos);
	SIZE_T copy=Min(size-pos, (SIZE_T)block);
	CopyMemory(&dst[dst_pos], &src[pos], copy);
	uWritePos+=copy;
	pos+=copy;
	cWritten.Broadcast();
	}
return pos;
}


//================
// Common Private
//================

UINT Intermediate::AllocBuffer(BYTE** buf_ptr, UINT* pos_ptr)
{
BYTE* buf=new BYTE[PAGE_SIZE];
cBuffers.append(buf);
*buf_ptr=buf;
*pos_ptr=0;
return PAGE_SIZE;
}

VOID Intermediate::DiscardBuffers()
{
UINT discard=(UINT)(uReadPos/PAGE_SIZE);
if(!discard)
	return;
SIZE_T discarded=discard*PAGE_SIZE;
for(auto it=cBuffers.begin(); discard>0; discard--)
	{
	auto buf=it.get_current();
	delete buf;
	it.remove_current();
	}
uReadPos-=discarded;
uWritePos-=discarded;
}

UINT Intermediate::GetBuffer(SIZE_T offset, BYTE** buf_ptr, UINT* pos_ptr)
{
UINT buf_id=(UINT)(offset/PAGE_SIZE);
UINT buf_count=cBuffers.get_count();
if(buf_id>=buf_count)
	return 0;
UINT buf_pos=(UINT)(offset%PAGE_SIZE);
*buf_ptr=cBuffers.get_at(buf_id);
*pos_ptr=buf_pos;
return PAGE_SIZE-buf_pos;
}

}
