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


//========
// Common
//========

VOID Intermediate::Clear()
{
WriteLock lock(m_Mutex);
m_Buffer.clear();
}


//==============
// Input-Stream
//==============

SIZE_T Intermediate::Available()
{
ReadLock lock(m_Mutex);
return m_Buffer.available();
}

SIZE_T Intermediate::Read(VOID* buf, SIZE_T size)
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
	SIZE_T read=m_Buffer.read(&dst[pos], copy);
	pos+=copy;
	}
return pos;
}


//===============
// Output-Stream
//===============

VOID Intermediate::Flush()
{
WriteLock lock(m_Mutex);
m_Buffer.flush();
m_Signal.Trigger();
}

SIZE_T Intermediate::Write(VOID const* buf, SIZE_T size)
{
WriteLock lock(m_Mutex);
return m_Buffer.write(buf, size);
}


//==========================
// Con-/Destructors Private
//==========================

Intermediate::Intermediate(UINT block_size):
m_Buffer(block_size)
{}

}