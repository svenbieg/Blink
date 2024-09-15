//=================
// DynamicBuffer.h
//=================

#pragma once


//=======
// Using
//=======

#include "Clusters/list.hpp"
#include "Seekable.h"


//===========
// Namespace
//===========

namespace Storage {


//================
// Dynamic Buffer
//================

class DynamicBuffer: public Seekable
{
public:
	// Con-/Destructors
	DynamicBuffer();

	// Access
	SIZE_T Available()override;
	FILE_SIZE GetSize()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;
	BOOL Seek(FILE_SIZE Position)override;

	// Modification
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	Clusters::list<BYTE, SIZE_T, 64> cList;
	Concurrency::Mutex cMutex;
	SIZE_T uPosition;
};

}
