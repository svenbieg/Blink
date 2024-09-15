//==================
// StaticBuffer.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "StaticBuffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

StaticBuffer::StaticBuffer(VOID const* buf, SIZE_T size):
pBuffer((BYTE*)buf),
uPosition(0),
uSize(size)
{}


//==============
// Input-Stream
//==============

SIZE_T StaticBuffer::Available()
{
if(uSize>uPosition)
	return uSize-uPosition;
return 0;
}

SIZE_T StaticBuffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=0;
if(uSize>uPosition)
	available=uSize-uPosition;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(buf)
	CopyMemory(buf, &pBuffer[uPosition], copy);
uPosition+=copy;
return copy;
}


//===============
// Output-Stream
//===============

VOID StaticBuffer::Flush()
{
}

SIZE_T StaticBuffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T available=0;
if(uSize>uPosition)
	available=uSize-uPosition;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
CopyMemory(&pBuffer[uPosition], buf, copy);
uPosition+=copy;
return copy;
}


//==========
// Seekable
//==========

BOOL StaticBuffer::Seek(FILE_SIZE pos)
{
if(uSize>0)
	{
	if(pos>=uSize)
		return false;
	}
uPosition=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

SIZE_T StaticBuffer::Fill(UINT value, SIZE_T size)
{
SIZE_T available=0;
if(uSize>uPosition)
	available=uSize-uPosition;
if(size==0)
	size=available;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(!copy)
	return 0;
FillMemory(&pBuffer[uPosition], copy, value);
uPosition+=copy;
return copy;
}

SIZE_T StaticBuffer::Zero(SIZE_T size)
{
SIZE_T available=0;
if(uSize>uPosition)
	available=uSize-uPosition;
if(size==0)
	size=available;
SIZE_T copy=size;
if(available)
	copy=Min(size, available);
if(!copy)
	return 0;
ZeroMemory(&pBuffer[uPosition], copy);
uPosition+=copy;
return copy;
}

}