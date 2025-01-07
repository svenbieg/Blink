//================
// Intermediate.h
//================

#pragma once


//=======
// Using
//=======

#include "Concurrency/Signal.h"
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
	~Intermediate();
	static Handle<Intermediate> Create(UINT BlockSize=PAGE_SIZE);

	// Common
	VOID Clear();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	Intermediate(UINT BlockSize);

	// Block
	struct BLOCK
		{
		BLOCK* Next;
		UINT Size;
		BYTE Buffer[];
		};

	// Common
	VOID* AllocateBlock();
	VOID CacheBlock(BLOCK* Block);
	VOID FreeBlocks(BLOCK* Block);
	UINT m_BlockSize;
	BLOCK* m_First;
	BLOCK* m_Free;
	BLOCK* m_Last;
	Concurrency::Mutex m_Mutex;
	UINT m_Offset;
	Concurrency::Signal m_Written;
};

}
