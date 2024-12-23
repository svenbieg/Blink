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


//=========
// Options
//=========

enum class BufferOptions
{
None=0,
Static=1
};


//========
// Buffer
//========

class Buffer: public Seekable
{
public:
	// Con-/Destructors
	Buffer(SIZE_T Size);
	Buffer(VOID const* Buffer, SIZE_T Size, BufferOptions Options=BufferOptions::None);
	~Buffer();

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
	// Common
	BYTE* m_Buffer;
	BufferOptions m_Options;
	SIZE_T m_Position;
	SIZE_T m_Size;
};

}
