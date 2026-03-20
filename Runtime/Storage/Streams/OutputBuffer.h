//================
// OutputBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Concurrency/CriticalSection.h"
#include "Handle.h"
#include "MemoryHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// Output-Buffer
//===============

class OutputBuffer: public Object
{
public:
	// Con-/Destructors
	~OutputBuffer()noexcept;
	static inline Handle<OutputBuffer> Create(SIZE_T BlockSize=MemoryHelper::PAGE_SIZE) { return new OutputBuffer(BlockSize); }

	// Common
	inline SIZE_T Available()const noexcept { return m_Written-m_Read; }
	VOID Clear()noexcept;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)noexcept;
	VOID Flush()noexcept;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);

private:
	// Block
	struct OutputBufferBlock
		{
		OutputBufferBlock* Next;
		SIZE_T Size;
		BYTE Buffer[];
		};

	// Con-/Destructors
	OutputBuffer(UINT BlockSize);

	// Common
	OutputBufferBlock* CreateBlock();
	VOID FreeBlock(OutputBufferBlock* Block)noexcept;
	VOID FreeBlocks(OutputBufferBlock* First)noexcept;
	SIZE_T m_BlockSize;
	Concurrency::CriticalSection m_CriticalSection;
	OutputBufferBlock* m_First;
	OutputBufferBlock* m_Free;
	OutputBufferBlock* m_Last;
	SIZE_T m_Read;
	SIZE_T m_Size;
	SIZE_T m_Written;
};

}}