//============
// Buffer.cpp
//============

#include "pch.h"


//=======
// Using
//=======

#include "Buffer.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

Buffer::Buffer(SIZE_T size):
pBuffer(new BYTE[size]),
uPosition(0),
uSize(size)
{}

Buffer::Buffer(VOID const* buf, SIZE_T size):
pBuffer(new BYTE[size]),
uPosition(0),
uSize(size)
{
CopyMemory(pBuffer, buf, size);
}

Buffer::~Buffer()
{
if(pBuffer)
	{
	delete pBuffer;
	pBuffer=nullptr;
	}
}


//==============
// Input-Stream
//==============

SIZE_T Buffer::Available()
{
return uSize-uPosition;
}

SIZE_T Buffer::Read(VOID* buf, SIZE_T size)
{
SIZE_T available=uSize-uPosition;
SIZE_T copy=Min(size, available);
if(buf)
	CopyMemory(buf, &pBuffer[uPosition], copy);
uPosition+=copy;
return copy;
}


//===============
// Output-Stream
//===============

VOID Buffer::Flush()
{
}

SIZE_T Buffer::Write(VOID const* buf, SIZE_T size)
{
SIZE_T available=uSize-uPosition;
SIZE_T copy=Min(size, available);
CopyMemory(&pBuffer[uPosition], buf, copy);
uPosition+=copy;
return copy;
}


//==========
// Seekable
//==========

BOOL Buffer::Seek(FILE_SIZE pos)
{
if(pos>uSize-1)
	return false;
uPosition=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

SIZE_T Buffer::Fill(UINT value, SIZE_T size)
{
SIZE_T available=uSize-uPosition;
if(size==0)
	size=available;
SIZE_T copy=Min(size, available);
FillMemory(&pBuffer[uPosition], copy, value);
uPosition+=copy;
return copy;
}

SIZE_T Buffer::Zero(SIZE_T size)
{
SIZE_T available=uSize-uPosition;
if(size==0)
	size=available;
SIZE_T copy=Min(size, available);
ZeroMemory(&pBuffer[uPosition], copy);
uPosition+=copy;
return copy;
}

}
