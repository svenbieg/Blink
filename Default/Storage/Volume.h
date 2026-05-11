//==========
// Volume.h
//==========

#pragma once


//=======
// Using
//=======

#include "Object.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Volume
//========

class Volume: public Object
{
public:
	// Common
	virtual VOID Erase(UINT64 Offset, UINT Size)=0;
	virtual WORD GetAlignment()=0;
	virtual UINT GetBlockSize()=0;
	virtual UINT GetPageSize()=0;
	virtual UINT64 GetSize()=0;
	virtual VOID Read(UINT64 Position, VOID* Buffer, SIZE_T Size)=0;
	virtual VOID SetSize(UINT64 Size)=0;
	virtual VOID Write(UINT64 Position, VOID const* Buffer, SIZE_T Size)=0;

protected:
	// Con-/Destructors
	Volume()=default;
};

}