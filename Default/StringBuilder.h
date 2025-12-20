//=================
// StringBuilder.h
//=================

#pragma once


//=======
// Using
//=======

#include "StringClass.h"


//================
// String-Builder
//================

class StringBuilder
{
public:
	// Con-/Destructors
	StringBuilder(UINT Length=0);
	~StringBuilder();

	// Common
	UINT Append(CHAR Char);
	UINT Append(WCHAR Char);
	UINT Append(LPCSTR String);
	UINT Append(LPCWSTR String);
	UINT Append(UINT Length, LPCSTR String);
	UINT Append(UINT Length, LPCWSTR String);
	VOID Clear();
	VOID Initialize(UINT Length);
	Handle<String> ToString();

private:
	// Settings
	static constexpr UINT STRING_BLOCK=64;

	// String-Block
	struct StringBlock
		{
		StringBlock(): Next(nullptr) {}
		StringBlock* Next;
		TCHAR Buffer[STRING_BLOCK];
		};

	// Functions
	typedef UINT (StringBuilder::*APPEND_ANSI)(CHAR);
	typedef UINT (StringBuilder::*APPEND_UNICODE)(WCHAR);
	typedef Handle<String> (StringBuilder::*TO_STRING)();

	UINT BufferAppend(TCHAR Char);
	UINT BufferAppendAnsi(CHAR Char);
	UINT BufferAppendUnicode(WCHAR Char);
	Handle<String> BufferToString();
	UINT StringAppendAnsi(CHAR Char);
	UINT StringAppendUnicode(WCHAR Char);
	Handle<String> StringToString();
	APPEND_ANSI m_AppendAnsi;
	APPEND_UNICODE m_AppendUnicode;
	StringBlock* m_First;
	StringBlock* m_Last;
	UINT m_Position;
	UINT m_Size;
	Handle<String> m_String;
	TO_STRING m_ToString;
};
