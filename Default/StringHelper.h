//================
// StringHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "CharHelper.h"
#include "MemoryHelper.h"
#include "VariableArguments.h"


//===============
// String-Helper
//===============

class StringHelper
{
public:
	// Format
	enum class Format
		{
		Char,
		Double,
		Float,
		Hex,
		Int,
		None,
		Percent,
		String,
		UInt
		};
	enum class FormatFlags: UINT
		{
		Left=1,
		None=0,
		Numeric=2,
		Precision=4,
		Signed=8,
		Space=16,
		Width=32,
		Zero=64
		};

	// Common
	static UINT Append(LPSTR Destination, UINT Size, LPCSTR String, LPCSTR Append);
	static INT Compare(LPCSTR Value1, LPCSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCSTR Value1, LPCWSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCWSTR Value1, LPCSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCWSTR Value1, LPCWSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
	static UINT Copy(LPSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
	static UINT Copy(LPSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
	static UINT Copy(LPWSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
	static UINT Copy(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
	static BOOL Find(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL Find(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL Find(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL Find(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChar(LPCSTR String, CHAR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChar(LPCWSTR String, CHAR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static UINT GetFormat(LPCSTR String, Format& Format, FormatFlags& Flags, UINT& Width, UINT& Precision);
	static UINT Insert(LPSTR Destination, UINT Size, LPCSTR String, UINT Position, LPCSTR Insert);
	static inline BOOL IsSet(LPCSTR String) { return String&&String[0]; }
	static inline BOOL IsSet(LPCWSTR String) { return String&&String[0]; }
	static UINT Length(LPCSTR String);
	static UINT Length(LPCWSTR String);
	static UINT Length(LPCSTR String, UINT Max);
	static UINT Length(LPCWSTR String, UINT Max);
	static UINT LowerCase(LPSTR Destination, UINT Size, LPCSTR String);
	template <class _char_t, class... _args_t>
	static inline UINT Print(_char_t* Buffer, UINT Size, LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		return StringHelper::PrintArgs(Buffer, Size, Format, vargs);
		}
	static UINT PrintArgs(LPSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);
	static UINT PrintArgs(LPWSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);
	static UINT PrintDouble(LPSTR Buffer, UINT Size, DOUBLE Value, FormatFlags Flags=FormatFlags::None, UINT Width=0, UINT Precision=0);
	static UINT PrintFloat(LPSTR Buffer, UINT Size, FLOAT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0, UINT Precision=0);
	static UINT PrintHex(LPSTR Buffer, UINT Size, UINT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	static UINT PrintHex(LPSTR Buffer, UINT Size, UINT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	static UINT PrintInt(LPSTR Buffer, UINT Size, INT Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	static UINT PrintInt(LPSTR Buffer, UINT Size, INT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	static UINT PrintUInt(LPSTR Buffer, UINT Size, UINT Value);
	static UINT PrintUInt(LPSTR Buffer, UINT Size, UINT Value, FormatFlags Flags, UINT Width=0);
	static UINT PrintUInt(LPSTR Buffer, UINT Size, UINT64 Value);
	static UINT PrintUInt(LPSTR Buffer, UINT Size, UINT64 Value, FormatFlags Flags=FormatFlags::None, UINT Width=0);
	static UINT Replace(LPSTR Destination, UINT Size, LPCSTR String, LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);
	static UINT Replace(LPWSTR Destination, UINT Size, LPCWSTR String, LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);
	template <class _char_t, class... _args_t>
	static inline UINT Scan(_char_t const* String, LPCSTR Format, _args_t... Arguments)
		{
		UnknownClass args[]={ Arguments... };
		VariableArguments vargs(args, ArraySize(args));
		return StringHelper::ScanArgs(String, Format, vargs);
		}
	static UINT ScanArgs(LPCSTR String, LPCSTR Format, VariableArguments& Arguments);
	static UINT ScanArgs(LPCWSTR String, LPCSTR Format, VariableArguments& Arguments);
	static UINT ScanFloat(LPCSTR String, FLOAT* Value);
	static UINT ScanFloat(LPCWSTR String, FLOAT* Value);
	static UINT ScanFloat(LPCSTR String, DOUBLE* Value);
	static UINT ScanFloat(LPCWSTR String, DOUBLE* Value);
	static UINT ScanInt(LPCSTR String, INT* Value);
	static UINT ScanInt(LPCWSTR String, INT* Value);
	static UINT ScanInt(LPCSTR String, INT64* Value);
	static UINT ScanInt(LPCWSTR String, INT64* Value);
	static UINT ScanUInt(LPCSTR String, UINT* Value, UINT Base=10, UINT Length=0);
	static UINT ScanUInt(LPCWSTR String, UINT* Value, UINT Base=10, UINT Length=0);
	static UINT ScanUInt(LPCSTR String, UINT64* Value, UINT Base=10, UINT Length=0);
	static UINT ScanUInt(LPCWSTR String, UINT64* Value, UINT Base=10, UINT Length=0);
	static LPCSTR Truncate(LPCSTR String, LPCSTR Chars);
	static LPCWSTR Truncate(LPCWSTR String, LPCSTR Chars);
	static UINT UpperCase(LPSTR Destination, UINT Size, LPCSTR String);
};
