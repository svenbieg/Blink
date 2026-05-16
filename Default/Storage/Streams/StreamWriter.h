//================
// StreamWriter.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/OutputStream.h"
#include "StringClass.h"


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
	StreamWriter(OutputStream* Stream=nullptr)noexcept;

	// Common
	VOID Flush();
	inline OutputStream* GetStream()const { return m_Stream; }
	UINT Print(LPCSTR Value);
	UINT Print(LPCWSTR Value);
	UINT Print(UINT Length, LPCSTR Value);
	UINT Print(UINT Length, LPCWSTR Value);
	UINT Print(String const* Value);
	UINT Print(LPCSTR Format, VariableArguments& Arguments);
	template <class... _args_t> inline UINT Print(LPCSTR Format, _args_t... Arguments)
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Print(Format, vargs);
		}
	UINT PrintChar(CHAR Char, UINT Count=1);
	UINT PrintChar(WCHAR Char, UINT Count=1);
	VOID SetStream(OutputStream* Stream)noexcept;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size);

private:
	// Common
	template <std::character _char_t> UINT DoPrint(UINT Length, _char_t const* Value);
	template <std::character _char_t> UINT DoPrintChar(_char_t Char, UINT Count);
	OutputStream* m_Stream;
};

}}