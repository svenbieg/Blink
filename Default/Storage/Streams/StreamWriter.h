//================
// StreamWriter.h
//================

#pragma once


//=======
// Using
//=======

#include "FlagHelper.h"
#include "OutputStream.h"
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
	StreamWriter(OutputStream* Stream);

	// Common
	UINT Print(LPCSTR Value);
	UINT Print(LPCSTR Value, StringFormatFlags Flags, UINT Width);
	UINT Print(LPCWSTR Value);
	UINT Print(LPCWSTR Value, StringFormatFlags Flags, UINT Width);
	UINT Print(UINT Length, LPCSTR Value);
	UINT Print(UINT Length, LPCWSTR Value);
	UINT Print(Handle<String> Value);
	UINT Print(LPCSTR Format, VariableArguments const& Arguments);
	template <class... _args_t> UINT Print(LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		return Print(Format, vargs);
		}
	UINT PrintChar(CHAR Char, UINT Count=1);
	UINT PrintChar(WCHAR Char, UINT Count=1);
	UINT PrintDouble(DOUBLE Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0, UINT Precision=0);
	UINT PrintFloat(FLOAT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0, UINT Precision=0);
	UINT PrintHex(UINT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	UINT PrintHex64(UINT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	UINT PrintInt(INT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	UINT PrintInt64(INT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	UINT PrintUInt(UINT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	UINT PrintUInt64(UINT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
	VOID SetStream(OutputStream* Stream);

private:
	// Using
	typedef UINT (*ANSI_PROC)(OutputStream* Stream, CHAR Char);
	typedef UINT (*UNICODE_PROC)(OutputStream* Stream, WCHAR Char);

	// Common
	template <class _func_t, class _char_t> UINT DoPrint(_func_t WriteChar, UINT Length, _char_t const* Value);
	template <class _char_t> UINT DoPrint(_char_t const* Value, StringFormatFlags Flags, UINT Width);
	template <class _func_t, class _char_t> UINT DoPrintChar(_func_t WriteChar, _char_t Char, UINT Count);

	OutputStream* m_Stream;
	ANSI_PROC m_WriteAnsi;
	UNICODE_PROC m_WriteUnicode;
};

}}
