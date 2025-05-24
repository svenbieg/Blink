//==========
// Volume.h
//==========

#pragma once


//=======
// Using
//=======

#include "Storage/Seekable.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Volume
//========

class Volume: public Seekable
{
public:
	// Common
	virtual SIZE_T Erase(FILE_SIZE Position, SIZE_T Size)=0;
	virtual UINT GetBlockSize()=0;
	virtual SIZE_T Read(FILE_SIZE Position, VOID* Buffer, SIZE_T Size)=0;
	virtual BOOL SetSize(FILE_SIZE Size)=0;
	virtual SIZE_T Write(FILE_SIZE Position, VOID const* Buffer, SIZE_T Size)=0;
};

}