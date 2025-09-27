//=================
// StringBuilder.h
//=================

#pragma once


//=======
// Using
//=======

#include "Storage/packet_buf.h"


//================
// String-Builder
//================

class StringBuilder
{
public:
	// Con-/Destructors
	StringBuilder();
	StringBuilder(UINT Length);

	// Common
	UINT Append(CHAR Char);
	UINT Append(WCHAR Char);
	UINT Append(LPCSTR String);
	UINT Append(LPCWSTR String);
	UINT Append(UINT Length, LPCSTR String);
	UINT Append(UINT Length, LPCWSTR String);
	Handle<String> ToString();

private:
	// Functions
	typedef UINT (StringBuilder::*APPEND_ANSI)(CHAR);
	typedef UINT (StringBuilder::*APPEND_UNICODE)(WCHAR);
	typedef VOID (StringBuilder::*INITIALIZE)();
	typedef Handle<String> (StringBuilder::*TO_STRING)();

	UINT BufferAppendAnsi(CHAR Char);
	UINT BufferAppendUnicode(WCHAR Char);
	VOID BufferInitialize();
	Handle<String> BufferToString();
	UINT StringAppendAnsi(CHAR Char);
	UINT StringAppendUnicode(WCHAR Char);
	VOID StringInitialize();
	Handle<String> StringToString();
	APPEND_ANSI m_AppendAnsi;
	APPEND_UNICODE m_AppendUnicode;
	Storage::packet_buf m_Buffer;
	INITIALIZE m_Initialize;
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_String;
	TO_STRING m_ToString;
};
