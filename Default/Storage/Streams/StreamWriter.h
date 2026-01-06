//================
// StreamWriter.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"
#include "FlagHelper.h"
#include "StringHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// String-Writer
//===============

class StreamWriter
{
public:
	// Con-/Destructors
	StreamWriter(OutputStream* Stream=nullptr);

	// Common
	VOID Flush();
	UINT Print(LPCSTR Value);
	UINT Print(LPCWSTR Value);
	UINT Print(UINT Length, LPCSTR Value);
	UINT Print(UINT Length, LPCWSTR Value);
	UINT Print(Handle<String> Value);
	UINT Print(LPCSTR Format, VariableArguments& Arguments);
	template <class... _args_t> inline UINT Print(LPCSTR Format, _args_t... Arguments)
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Print(Format, vargs);
		}
	UINT PrintChar(CHAR Char, UINT Count=1);
	UINT PrintChar(WCHAR Char, UINT Count=1);
	VOID SetStream(OutputStream* Stream);
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);

private:
	// Using
	typedef UINT (*ANSI_PROC)(OutputStream* Stream, CHAR Char);
	typedef UINT (*UNICODE_PROC)(OutputStream* Stream, WCHAR Char);

	// Common
	template <class _func_t, class _char_t> UINT DoPrint(_func_t WriteChar, UINT Length, _char_t const* Value);
	template <class _func_t, class _char_t> UINT DoPrintChar(_func_t WriteChar, _char_t Char, UINT Count);
	VOID SetStreamFormat(StreamFormat Format);

	OutputStream* m_Stream;
	ANSI_PROC m_WriteAnsi;
	UNICODE_PROC m_WriteUnicode;
};

}}