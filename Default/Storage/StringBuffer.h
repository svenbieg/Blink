//================
// StringBuffer.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"


//===========
// Namespace
//===========

namespace Storage {


//===============
// String-Buffer
//===============

class StringBuffer: public Streams::InputStream
{
public:
	// Con-/Destructors
	static inline Handle<StringBuffer> Create(Handle<String> Value) { return new StringBuffer(Value); }

	// Common
	VOID Reset() { m_Position=0; }

	// Input-Stream
	SIZE_T Available()override { return m_Size-m_Position; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	StringBuffer(Handle<String> Value);

	// Common
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_Value;
};

}