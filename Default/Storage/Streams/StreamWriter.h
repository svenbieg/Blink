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
	// Using
	using Format=StringHelper::Format;
	using FormatFlags=StringHelper::FormatFlags;

	// Con-/Destructors
	StreamWriter(OutputStream* Stream=nullptr);

	// Common
	VOID Flush();
	UINT Print(LPCSTR Value);
	UINT Print(LPCSTR Value, FormatFlags Flags, UINT Width);
	UINT Print(LPCWSTR Value);
	UINT Print(LPCWSTR Value, FormatFlags Flags, UINT Width);
	UINT Print(UINT Length, LPCSTR Value);
	UINT Print(UINT Length, LPCWSTR Value);
	UINT Print(Handle<String> Value);
	UINT Print(LPCSTR Format, VariableArguments& Arguments);
	template <class... _args_t> UINT Print(LPCSTR Format, _args_t... Arguments)
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Print(Format, vargs);
		}
	UINT PrintChar(CHAR Char, UINT Count=1);
	UINT PrintChar(WCHAR Char, UINT Count=1);
	UINT PrintDouble(DOUBLE Value, FormatFlags Flags=FormatFlags::None, UINT Width=0, UINT Precision=0);
	UINT PrintFloat(FLOAT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0, UINT Precision=0);
	UINT PrintHex(UINT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	UINT PrintHex(UINT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	UINT PrintInt(INT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	UINT PrintInt(INT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	UINT PrintUInt(UINT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	UINT PrintUInt(UINT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	VOID SetStream(OutputStream* Stream);
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);

private:
	// Using
	typedef UINT (*ANSI_PROC)(OutputStream* Stream, CHAR Char);
	typedef UINT (*UNICODE_PROC)(OutputStream* Stream, WCHAR Char);

	// Common
	template <class _func_t, class _char_t> UINT DoPrint(_func_t WriteChar, UINT Length, _char_t const* Value);
	template <class _char_t> UINT DoPrint(_char_t const* Value, FormatFlags Flags, UINT Width);
	template <class _func_t, class _char_t> UINT DoPrintChar(_func_t WriteChar, _char_t Char, UINT Count);
	VOID SetStreamFormat(StreamFormat Format);

	OutputStream* m_Stream;
	ANSI_PROC m_WriteAnsi;
	UNICODE_PROC m_WriteUnicode;
};

}}