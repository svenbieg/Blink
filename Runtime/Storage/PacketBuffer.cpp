//==================
// PacketBuffer.cpp
//==================

#include "PacketBuffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==============
// Input-Stream
//==============

SIZE_T PacketBuffer::Available()
{
return m_Size-m_Read;
}

SIZE_T PacketBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Read);
if(buf)
	MemoryHelper::Copy(buf, &m_Buffer[m_Read], copy);
m_Read+=copy;
return copy;
}


//===============
// Output-Buffer
//===============

VOID PacketBuffer::Flush()
{
}

SIZE_T PacketBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T copy=TypeHelper::Min(size, m_Size-m_Written);
MemoryHelper::Copy(&m_Buffer[m_Written], buf, copy);
m_Written+=copy;
return copy;
}


//============================
// Con-/Destructors Protected
//============================

PacketBuffer::PacketBuffer(BYTE* buf, SIZE_T size):
m_Buffer(buf),
m_Read(0),
m_Size(size),
m_Written(0)
{}

}