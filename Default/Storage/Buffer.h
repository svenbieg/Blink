//==========
// Buffer.h
//==========

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


//========
// Buffer
//========

class Buffer: public Streams::Seekable
{
public:
	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<Buffer> Create(SIZE_T Size, SIZE_T Align=sizeof(SIZE_T)) { return Object::CreateEx<Buffer>(Size, Align); }

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Seekable
	SIZE_T GetSize()override;
	BOOL Seek(SIZE_T Position)override;

	// Access
	inline BYTE* Begin()const { return m_Buffer; }

	// Modification
	SIZE_T Fill(UINT Value, SIZE_T Size=0);

private:
	// Con-/Destructors
	Buffer(VOID* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}