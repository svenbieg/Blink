//================
// StreamReader.h
//================

#pragma once


//=======
// Using
//=======

#include "Clusters/list.hpp"
#include "CharHelper.h"


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
	StreamReader(InputStream* Stream);

	// Common
	UINT FindChar(LPCSTR Chars);
	TCHAR LastChar;
	UINT ReadString(LPSTR Buffer, UINT Size);
	UINT ReadString(LPWSTR Buffer, UINT Size);
	UINT ReadString(LPSTR Buffer, UINT Size, CHAR Escape);
	UINT ReadString(LPWSTR Buffer, UINT Size, CHAR Escape);
	UINT ReadString(LPSTR Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate=nullptr);
	UINT ReadString(LPWSTR Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate=nullptr);
	Handle<String> ReadString(SIZE_T* Size=nullptr, LPCSTR Escape=nullptr, LPCSTR Truncate=nullptr);
	VOID SetStream(InputStream* Stream);
	UINT Skip(UINT Count=1);

private:
	// Using
	typedef UINT (*ANSI_PROC)(InputStream* Stream, CHAR* Char);
	typedef UINT (*UNICODE_PROC)(InputStream* Stream, WCHAR* Char);

	// Common
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size);
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size, CHAR Escape);
	template <class _func_t, class _char_t> UINT DoReadString(_func_t ReadChar, _char_t* Buffer, UINT Size, LPCSTR Escape, LPCSTR Truncate);
	template <class _func_t> Handle<String> DoReadString(_func_t ReadChar, SIZE_T* Size, LPCSTR Escape, LPCSTR Truncate);
	ANSI_PROC m_ReadAnsi;
	UNICODE_PROC m_ReadUnicode;
	InputStream* m_Stream;
};

}}
