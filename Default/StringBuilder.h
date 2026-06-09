//=================
// StringBuilder.h
//=================

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/StreamBuffer.h"
#include "StringClass.h"


//================
// String-Builder
//================

class StringBuilder
{
public:
	// Using
	using StreamBuffer=Storage::Streams::StreamBuffer;

	// Con-/Destructors
	StringBuilder(UINT Length=0);

	// Common
	UINT Append(CHAR Char);
	UINT Append(WCHAR Char);
	UINT Append(LPCSTR String);
	UINT Append(LPCWSTR String);
	UINT Append(UINT Length, LPCSTR String);
	UINT Append(UINT Length, LPCWSTR String);
	VOID Clear()noexcept;
	VOID Initialize(UINT Length);
	Handle<String> ToString();

private:
	// Functions
	typedef UINT (StringBuilder::*APPEND_FN)(WCHAR);
	typedef Handle<String> (StringBuilder::*TO_STRING_FN)();

	UINT BufferAppend(WCHAR Char);
	Handle<String> BufferToString();
	UINT StringAppend(WCHAR Char);
	Handle<String> StringToString();
	APPEND_FN m_Append;
	Handle<StreamBuffer> m_Buffer;
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_String;
	TO_STRING_FN m_ToString;
};
