//==================
// PacketBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/PacketBuffer.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

VOID PacketBuffer::Clear()
{
WriteLock lock(m_Mutex);
m_Buffer.clear();
}


//==============
// Input-Stream
//==============

SIZE_T PacketBuffer::Available()
{
ReadLock lock(m_Mutex);
return m_Buffer.available();
}

SIZE_T PacketBuffer::Read(VOID* buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
auto dst=(BYTE*)buf;
SIZE_T pos=0;
while(pos<size)
	{
	SIZE_T available=m_Buffer.available();
	if(!available)
		{
		m_Signal.Wait(lock);
		continue;
		}
	SIZE_T copy=TypeHelper::Min(available, size-pos);
	m_Buffer.read(&dst[pos], copy);
	pos+=copy;
	}
return pos;
}


//===============
// Output-Stream
//===============

VOID PacketBuffer::Flush()
{
WriteLock lock(m_Mutex);
m_Buffer.flush();
m_Signal.Trigger();
}

SIZE_T PacketBuffer::Write(VOID const* buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
return m_Buffer.write(buf, size);
}


//==========================
// Con-/Destructors Private
//==========================

PacketBuffer::PacketBuffer(UINT block_size):
m_Buffer(block_size)
{}

}