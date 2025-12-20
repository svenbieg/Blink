//==========
// Buffer.h
//==========

#pragma once


//=======
// Using
//=======

#include "Storage/Seekable.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Buffer
//========

class Buffer: public Seekable
{
public:
	// Con-/Destructors
	static Handle<Buffer> Create(SIZE_T Size);

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	inline VOID Flush()override {}
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Seekable
	inline FILE_SIZE GetSize()override { return m_Size; }
	BOOL Seek(FILE_SIZE Position)override;

	// Access
	inline BYTE* Begin()const { return m_Buffer; }

	// Modification
	SIZE_T Fill(UINT Value, SIZE_T Size=0);

private:
	// Con-/Destructors
	Buffer(BYTE* Buffer, SIZE_T Size);

	// Common
	BYTE* m_Buffer;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}