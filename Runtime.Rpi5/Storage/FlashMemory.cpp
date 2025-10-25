//=================
// FlashMemory.cpp
//=================

#include "pch.h"


//=======
// Using
//=======

#include "Storage/FlashMemory.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Volume
//========

UINT FlashMemory::GetBlockSize()
{
return m_BlockSize;
}

FILE_SIZE FlashMemory::GetSize()
{
return m_Size;
}

SIZE_T FlashMemory::Read(FILE_SIZE pos, VOID* buf, SIZE_T size)
{
FILE_SIZE block_count=size/m_BlockSize;
if(!block_count)
	return 0;
FILE_SIZE offset=m_Offset+pos;
FILE_SIZE block=offset/m_BlockSize;
m_Device->Read(block, block_count, m_BlockSize, (UINT*)buf);
return size;
}

BOOL FlashMemory::SetSize(FILE_SIZE size)
{
return size<=m_Size;
}

SIZE_T FlashMemory::Write(FILE_SIZE pos, VOID const* buf, SIZE_T size)
{
FILE_SIZE block_count=size/m_BlockSize;
if(!block_count)
	return 0;
FILE_SIZE offset=m_Offset+pos;
FILE_SIZE block=offset/m_BlockSize;
m_Device->Write(block, block_count, m_BlockSize, (UINT const*)buf);
return size;
}


//==========================
// Con-/Destructors Private
//==========================

FlashMemory::FlashMemory(EmmcHost* device, FILE_SIZE offset, FILE_SIZE size):
m_BlockSize(device->GetBlockSize()),
m_Device(device),
m_Offset(offset),
m_Size(size)
{}

}