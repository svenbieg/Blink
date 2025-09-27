//================
// StaticBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Seekable.h"


//===========
// Namespace
//===========

namespace Storage {


//===============
// Static-Buffer
//===============

class StaticBuffer: public Seekable
{
public:
	// Con-/Destructors
	static inline Handle<StaticBuffer> Create(VOID* Buffer, SIZE_T Size) { return new StaticBuffer(Buffer, Size); }

	// Common
	VOID Reset() { m_Position=0; }

	// Input-Stream
	SIZE_T Available()override { return m_Size-m_Position; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Ouput-Stream
	inline VOID Flush()override {}
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Seekable
	FILE_SIZE GetSize()override { return m_Size; }
	BOOL Seek(FILE_SIZE Position)override;

private:
	// Con-/Destructors
	StaticBuffer(VOID* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}