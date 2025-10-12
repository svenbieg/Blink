//==============
// StringView.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"


//=============
// String-View
//=============

class StringView: public Storage::Streams::InputStream
{
public:
	// Con-/Destructors
	static inline Handle<StringView> Create(Handle<String> Value) { return new StringView(Value); }

	// Common
	VOID Reset() { m_Position=0; }

	// Input-Stream
	SIZE_T Available()override { return m_Size-m_Position; }
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

private:
	// Con-/Destructors
	StringView(Handle<String> Value);

	// Common
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_Value;
};
