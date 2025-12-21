//================
// StreamReader.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"
#include "CharHelper.h"
#include "StringHelper.h"


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
	StreamReader(IInputStream* Stream);

	// Common
	UINT FindChar(LPCSTR Chars);
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
	VOID SetStream(IInputStream* Stream);
	UINT Skip(UINT Count=1);
	UINT Skip(LPCSTR Chars);

private:
	// Using
	typedef UINT (*ANSI_PROC)(IInputStream* Stream, CHAR* Char);
	typedef UINT (*UNICODE_PROC)(IInputStream* Stream, WCHAR* Char);

	// Common
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size);
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size, CHAR Escape);
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate);
	template <class _func_t> Handle<String> DoReadString(_func_t ReadChar, SIZE_T* Size, LPCSTR Escape, LPCSTR Truncate);
	ANSI_PROC m_ReadAnsi;
	UNICODE_PROC m_ReadUnicode;
	IInputStream* m_Stream;
};

}}