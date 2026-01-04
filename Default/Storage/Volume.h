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
	virtual UINT GetBlockSize()=0;
	virtual UINT64 GetSize()=0;
	virtual SIZE_T Read(UINT64 Position, VOID* Buffer, SIZE_T Size)=0;
	virtual BOOL SetSize(UINT64 Size)=0;
	virtual SIZE_T Write(UINT64 Position, VOID const* Buffer, SIZE_T Size)=0;

protected:
	// Con-/Destructors
	Volume()=default;
};

}