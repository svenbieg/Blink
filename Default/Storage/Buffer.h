//==========
// Buffer.h
//==========

#pragma once


//=======
// Using
//=======

#include "Seekable.h"


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
	Buffer(SIZE_T Size);
	Buffer(VOID const* Buffer, SIZE_T Size);
	~Buffer();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

	// Seekable
	inline FILE_SIZE GetSize()override { return m_Size; }
	BOOL Seek(FILE_SIZE Position)override;

	// Access
	inline BYTE* Begin()const { return m_Buffer; }

	// Modification
	SIZE_T Fill(UINT Value, SIZE_T Size=0);
	SIZE_T Zero(SIZE_T Size=0);

private:
	// Common
	BYTE* m_Buffer;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}
