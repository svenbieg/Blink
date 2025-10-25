//==============
// ReadBuffer.h
//==============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Task.h"
#include "Storage/Streams/RandomAccessStream.h"
#include "Storage/RingBuffer.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//=============
// Read-Buffer
//=============

class ReadBuffer: public Object
{
public:
	// Con-/Destructors
	~ReadBuffer();
	static inline Handle<ReadBuffer> Create(SIZE_T BlockSize=PAGE_SIZE) { return new ReadBuffer(BlockSize); }

	// Common
	SIZE_T Available();
	VOID Clear();
	VOID Flush();
	SIZE_T Read(VOID* Buffer, SIZE_T Size);
	SIZE_T Write(RingBuffer* Buffer, SIZE_T Size);

private:
	// Block
	struct ReadBufferBlock
		{
		ReadBufferBlock* Next;
		SIZE_T Size;
		BYTE Buffer[];
		};

	// Con-/Destructors
	ReadBuffer(UINT BlockSize);

	// Common
	ReadBufferBlock* CreateBlock();
	VOID FreeBlocks(ReadBufferBlock* First);
	SIZE_T m_BlockSize;
	ReadBufferBlock* m_First;
	ReadBufferBlock* m_Last;
	Concurrency::Mutex m_Mutex;
	SIZE_T m_Read;
	Concurrency::Signal m_Signal;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}}