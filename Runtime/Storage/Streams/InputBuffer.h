//===============
// InputBuffer.h
//===============

#pragma once


//=======
// Using
//=======

#include "Concurrency/Scheduler.h"
#include "Storage/Streams/Stream.h"
#include "Storage/RingBuffer.h"
#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==============
// Input-Buffer
//==============

class InputBuffer: public Object
{
public:
	// Con-/Destructors
	~InputBuffer();
	static inline Handle<InputBuffer> Create(SIZE_T BlockSize=MemoryHelper::PAGE_SIZE) { return new InputBuffer(BlockSize); }

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
	InputBuffer(UINT BlockSize);

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