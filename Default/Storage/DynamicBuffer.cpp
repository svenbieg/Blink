//===================
// DynamicBuffer.cpp
//===================

#include "pch.h"


//=======
// Using
//=======

#include "DynamicBuffer.h"

using namespace Concurrency;


//===========
// Namespace
//===========

namespace Storage {


//==================
// Con-/Destructors
//==================

DynamicBuffer::DynamicBuffer():
uPosition(0)
{}


//========
// Access
//========

SIZE_T DynamicBuffer::Available()
{
SharedLock lock(cMutex);
return cList.get_count()-uPosition;
}

FILE_SIZE DynamicBuffer::GetSize()
{
SharedLock lock(cMutex);
return cList.get_count();
}

SIZE_T DynamicBuffer::Read(VOID* buf, SIZE_T size)
{
SharedLock lock(cMutex);
SIZE_T read=cList.get_many(uPosition, (BYTE*)buf, size);
uPosition+=read;
return read;
}

BOOL DynamicBuffer::Seek(FILE_SIZE pos)
{
ScopedLock lock(cMutex);
if(pos>=cList.get_count())
	return false;
uPosition=(SIZE_T)pos;
return true;
}


//==============
// Modification
//==============

VOID DynamicBuffer::Flush()
{
ScopedLock lock(cMutex);
uPosition=0;
}

SIZE_T DynamicBuffer::Write(VOID const* buf, SIZE_T size)
{
ScopedLock lock(cMutex);
cList.set_many(uPosition, (BYTE const*)buf, size);
uPosition+=size;
return size;
}

}
