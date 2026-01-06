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
	// Common
	static UINT Append(LPSTR Destination, UINT Size, LPCSTR Source, LPCSTR Value);
	static UINT Append(LPWSTR Destination, UINT Size, LPCSTR Source, LPCSTR Value);
	static INT Compare(LPCSTR String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCSTR String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCWSTR String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCWSTR String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCSTR String1, String const* String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(LPCWSTR String1, String const* String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(String const* String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(String const* String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true);
	static INT Compare(String const* Value1, String const* Value2);
	static INT Compare(String const* Value1, String const* Value2, UINT Length, BOOL CaseSensitive=true);
	static UINT Copy(LPSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
	static UINT Copy(LPSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
	static UINT Copy(LPWSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
	static UINT Copy(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
	static UINT Decrypt(LPSTR Destination, UINT Size, BYTE const* Source, LPCSTR Key);
	static UINT Encrypt(BYTE* Destination, SIZE_T Size, LPCSTR Source, LPCSTR Key);
	static BOOL FindChar(LPCSTR String, CHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChar(LPCSTR String, WCHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChar(LPCWSTR String, CHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChar(LPCWSTR String, WCHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindChars(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindString(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindString(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindString(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static BOOL FindString(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static UINT64 Hash(LPCSTR String);
	static UINT64 Hash(LPCWSTR String);
	static UINT Insert(LPSTR Destination, UINT Size, LPCSTR Source, UINT Position, LPCSTR Value);
	static UINT Insert(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Position, LPCSTR Value);
	static inline BOOL IsSet(LPCSTR String) { return String&&String[0]; }
	static inline BOOL IsSet(LPCWSTR String) { return String&&String[0]; }
	static UINT Length(LPCSTR String);
	static UINT Length(LPCWSTR String);
	static UINT Length(LPCSTR String, UINT Max);
	static UINT Length(LPCWSTR String, UINT Max);
	static UINT Length(LPCSTR Format, VariableArguments& Arguments);
	static UINT Lowercase(LPSTR String);
	static UINT Lowercase(LPWSTR String);
	static UINT Lowercase(LPSTR Destination, UINT Size, LPCSTR Source);
	static UINT Lowercase(LPWSTR Destination, UINT Size, LPCWSTR Source);
	static UINT Print(LPSTR Destination, UINT Size, LPCSTR Format, VariableArguments& Arguments);
	static UINT Print(LPWSTR Destination, UINT Size, LPCSTR Format, VariableArguments& Arguments);
	template <std::character _dst_t, class... _args_t> static inline UINT Print(_dst_t* Destination, UINT Size, LPCSTR Format, _args_t... Arguments)
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Print(Destination, Size, Format, vargs);
		}
	static UINT Replace(LPSTR Destination, UINT Size, LPCSTR Source, LPCSTR Find, LPCSTR Insert, BOOL CaseSensitive, BOOL Repeat);
	static UINT Replace(LPWSTR Destination, UINT Size, LPCWSTR Source, LPCSTR Find, LPCSTR Insert, BOOL CaseSensitive, BOOL Repeat);
	static UINT Scan(LPCSTR String, LPCSTR Format, VariableArguments& Arguments);
	static UINT Scan(LPCWSTR String, LPCSTR Format, VariableArguments& Arguments);
	template <std::character _src_t, class... _args_t> static inline UINT Scan(_src_t const* String, LPCSTR Format, _args_t... Arguments)
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Scan(String, Format, vargs);
		}
	static LPCSTR Truncate(LPCSTR String, LPCSTR Characters);
	static LPCWSTR Truncate(LPCWSTR String, LPCSTR Characters);
	static UINT Uppercase(LPSTR String);
	static UINT Uppercase(LPWSTR String);
	static UINT Uppercase(LPSTR Destination, UINT Size, LPCSTR Source);
	static UINT Uppercase(LPWSTR Destination, UINT Size, LPCWSTR Source);

private:
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

	// Format-Flags
	enum class FormatFlags: UINT
		{
		None=0,
		High=1,
		Left=2,
		Low=4,
		Numeric=8,
		Precision=16,
		Signed=32,
		Space=64,
		Width=128,
		Zero=64
		};

	// Common
	template <std::character _dst_t, std::character _src_t, std::character _append_t> static UINT Append(_dst_t* Destination, UINT Size, _src_t const* Source, _append_t const* Append);
	template <std::character _str1_t, std::character _str2_t> static INT Compare(_str1_t const* String1, _str2_t const* String2, UINT Length=0, BOOL CaseSensitive=true);
	template <std::character _str_t> static INT Compare(String const* Value1, _str_t const* Value2, UINT Length=0, BOOL CaseSensitive=true);
	template <std::character _str_t> static INT Compare(_str_t const* Value1, String const* Value2, UINT Length=0, BOOL CaseSensitive=true);
	template <std::character _dst_t, std::character _src_t> static UINT Copy(_dst_t* Destination, UINT Size, _src_t const* Source, UINT Length=0);
	template <std::character _str_t, std::character _find_t> static BOOL FindChar(_str_t const* String, _find_t Char, UINT* Position=nullptr, BOOL CaseSensitive=true);
	template <std::character _str_t, std::character _find_t> static BOOL FindChars(_str_t const* String, _find_t const* Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	template <std::character _str_t, std::character _find_t> static BOOL FindString(_str_t const* String, _find_t const* Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
	static UINT GetFormat(LPCSTR String, Format& Format, FormatFlags& Flags, UINT& Width, UINT& Precision);
	template <std::character _str_t> static UINT64 Hash(_str_t const* String);
	template <std::character _dst_t, std::character _src_t, std::character _insert_t> static UINT Insert(_dst_t* Destination, UINT Size, _src_t const* Source, UINT Position, _insert_t const* Insert);
	template <std::character _str_t> static UINT Length(_str_t const* Value);
	template <std::character _str_t> static UINT Length(_str_t const* Value, UINT Max);
	template <std::character _str_t> static UINT Lowercase(_str_t* String);
	template <std::character _dst_t, std::character _src_t> static UINT Lowercase(_dst_t* Destination, UINT Size, _src_t const* Source);
	template <std::character _dst_t> static UINT Print(_dst_t* Destination, UINT Size, LPCSTR Format, VariableArguments& Arguments);
	template <std::character _dst_t, std::character _char_t> static UINT PrintChar(_dst_t* Destination, UINT Size, _char_t Char, UINT Position);
	template <std::character _dst_t, std::character _char_t> static UINT PrintChar(_dst_t* Destination, UINT Size, _char_t Char, FormatFlags Format, UINT Position);
	template <std::character _dst_t, std::character _char_t> static UINT PrintChars(_dst_t* Destination, UINT Size, _char_t Char, UINT Count, UINT Position);
	template <std::character _dst_t, std::floating_point _float_t> static UINT PrintFloat(_dst_t* Destination, UINT Size, _float_t Float, FormatFlags Format=FormatFlags::None, UINT Width=0, UINT Precision=0, UINT Position=0);
	template <std::character _dst_t, std::unsigned_integral _uint_t> static UINT PrintHex(_dst_t* Destination, UINT Size, _uint_t Value, UINT Position);
	template <std::character _dst_t, std::unsigned_integral _uint_t> static UINT PrintHex(_dst_t* Destination, UINT Size, _uint_t Value, FormatFlags Format, UINT Width, UINT Position);
	template <std::character _dst_t, std::signed_integral _int_t> static UINT PrintInt(_dst_t* Destination, UINT Size, _int_t Value, FormatFlags Format, UINT Width, UINT Position);
	template <std::character _dst_t, std::character _src_t> static UINT PrintString(_dst_t* Destination, UINT Size, _src_t const* Source, UINT Position);
	template <std::character _dst_t, std::character _src_t> static UINT PrintString(_dst_t* Destination, UINT Size, _src_t const* Source, FormatFlags Format, UINT Width, UINT Position);
	template <std::character _dst_t, std::character _src_t> static UINT PrintStringCapital(_dst_t* Destination, UINT Size, _src_t const* Source, UINT Position);
	template <std::character _dst_t, std::character _src_t> static UINT PrintStringSmall(_dst_t* Destination, UINT Size, _src_t const* Source, UINT Position);
	template <std::character _dst_t, std::unsigned_integral _uint_t> static UINT PrintUInt(_dst_t* Destination, UINT Size, _uint_t Value, UINT Position);
	template <std::character _dst_t, std::unsigned_integral _uint_t> static UINT PrintUInt(_dst_t* Destination, UINT Size, _uint_t Value, FormatFlags Format, UINT Width, UINT Position);
	template <std::character _dst_t, std::character _src_t, std::character _find_t, std::character _insert_t> static UINT Replace(_dst_t* Destination, UINT Size, _src_t const* Source, _find_t const* Find, _insert_t const* Insert, BOOL CaseSensitive, BOOL Repeat);
	template <std::character _str_t> static UINT Scan(_str_t const* String, LPCSTR Format, VariableArguments& Arguments);
	template <std::character _str_t, std::floating_point _float_t> static UINT ScanFloat(_str_t const* String, _float_t* Value);
	template <std::character _str_t, std::signed_integral _int_t> static UINT ScanInt(_str_t const* String, _int_t* Value);
	template <std::character _str_t, std::character _buf_t> static UINT ScanString(_str_t const* String, _buf_t* Value, UINT Size, CHAR Escape);
	template <std::character _str_t, std::unsigned_integral _uint_t> static UINT ScanUInt(_str_t const* String, _uint_t* Value, UINT Base=10, UINT Length=0);
	template <std::character _str_t> static _str_t const* Truncate(_str_t const* String, LPCSTR Characters);
	template <std::character _str_t> static UINT Uppercase(_str_t* String);
	template <std::character _dst_t, std::character _src_t> static UINT Uppercase(_dst_t* Destination, UINT Size, _src_t const* Source);
};
