//================
// Intermediate.h
//================

#pragma once


//=======
// Using
//=======

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

	// Common
	VOID Clear();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Buffer
	struct BUFFER
		{
		BYTE Buffer[PAGE_SIZE];
		BUFFER* Next;
		UINT Size;
		};

	// Common
	BUFFER* m_First;
	BUFFER* m_Last;
	Concurrency::Mutex m_Mutex;
	UINT m_Offset;
	Concurrency::Signal m_Written;
};

}
