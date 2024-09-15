//================
// Intermediate.h
//================

#pragma once


//=======
// Using
//=======

#include "Clusters/list.hpp"
#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Storage {


//==============
// Intermediate
//==============

class Intermediate: public Streams::RandomAccessStream
{
public:
	// Con-/Destructors
	Intermediate();
	~Intermediate();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override {}
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Common
	UINT AllocBuffer(BYTE** Buffer, UINT* Position);
	VOID DiscardBuffers();
	UINT GetBuffer(SIZE_T Offset, BYTE** Buffer, UINT* Position);
	Clusters::list<BYTE*> cBuffers;
	Concurrency::Mutex cMutex;
	Concurrency::Signal cWritten;
	SIZE_T uReadPos;
	SIZE_T uWritePos;
};

}
