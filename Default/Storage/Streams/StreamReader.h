//================
// StreamReader.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "StringClass.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// String-Reader
//===============

class StreamReader
{
public:
	// Con-/Destructors
	StreamReader(InputStream* Stream)noexcept;

	// Common
	SIZE_T Available();
	UINT FindChar(LPCSTR Chars);
	inline InputStream* GetStream()const { return m_Stream; }
	TCHAR LastChar;
	SIZE_T Read(VOID* Buffer, SIZE_T Size);
	UINT ReadChar(CHAR* Char=nullptr);
	UINT ReadChar(WCHAR* Char);
	UINT ReadString(LPSTR Buffer, UINT Size);
	UINT ReadString(LPWSTR Buffer, UINT Size);
	UINT ReadString(LPSTR Buffer, UINT Size, CHAR Escape);
	UINT ReadString(LPWSTR Buffer, UINT Size, CHAR Escape);
	UINT ReadString(LPSTR Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate=nullptr);
	UINT ReadString(LPWSTR Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate=nullptr);
	Handle<String> ReadString(SIZE_T* Size=nullptr, LPCSTR Escape=nullptr, LPCSTR Truncate=nullptr);
	VOID SetStream(InputStream* Stream)noexcept;
	UINT Skip(UINT Count=1);
	UINT Skip(LPCSTR Chars);

private:
	// Common
	template <std::character _char_t> UINT DoReadString(_char_t* Buffer, UINT Size);
	template <std::character _char_t> UINT DoReadString(_char_t* Buffer, UINT Size, CHAR Escape);
	template <std::character _char_t> UINT DoReadString(_char_t* Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate);
	Handle<String> DoReadString(SIZE_T* Size, LPCSTR Escape, LPCSTR Truncate);
	InputStream* m_Stream;
};

}}