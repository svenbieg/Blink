//================
// StaticBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {


//===============
// Static-Buffer
//===============

class StaticBuffer: public Streams::Seekable
{
public:
	// Con-/Destructors
	static inline Handle<StaticBuffer> Create(VOID* Buffer, SIZE_T Size) { return new StaticBuffer(Buffer, Size); }

	// Common
	inline VOID Reset() { m_Position=0; }

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Seekable
	SIZE_T GetSize()override;
	BOOL Seek(SIZE_T Position)override;

private:
	// Con-/Destructors
	StaticBuffer(VOID* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}