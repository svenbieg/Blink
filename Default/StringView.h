//==============
// StringView.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"
#include "StringClass.h"


//=============
// String-View
//=============

class StringView: public Storage::Streams::IInputStream
{
public:
	// Con-/Destructors
	StringView(Handle<String> String);

	// Common
	VOID Reset();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

private:
	// Common
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_Value;
};
