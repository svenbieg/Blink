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
	static UINT Append(LPSTR Destination, UINT Size, LPCSTR Source, LPCSTR Value)noexcept;
	static UINT Append(LPWSTR Destination, UINT Size, LPCSTR Source, LPCSTR Value)noexcept;
	static INT Compare(LPCSTR String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(LPCSTR String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(LPCWSTR String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(LPCWSTR String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(LPCSTR String1, String const* String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(LPCWSTR String1, String const* String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(String const* String1, LPCSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(String const* String1, LPCWSTR String2, UINT Length=0, BOOL CaseSensitive=true)noexcept;
	static INT Compare(String const* Value1, String const* Value2)noexcept;
	static INT Compare(String const* Value1, String const* Value2, UINT Length, BOOL CaseSensitive=true)noexcept;
	static UINT Copy(LPSTR Destination, UINT Size, LPCSTR Source, UINT Length=0)noexcept;
	static UINT Copy(LPSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0)noexcept;
	static UINT Copy(LPWSTR Destination, UINT Size, LPCSTR Source, UINT Length=0)noexcept;
	static UINT Copy(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0)noexcept;
	static UINT Decrypt(LPSTR Destination, UINT Size, BYTE const* Source, LPCSTR Key)noexcept;
	static UINT Encrypt(BYTE* Destination, SIZE_T Size, LPCSTR Source, LPCSTR Key);
	static BOOL FindChar(LPCSTR String, CHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChar(LPCSTR String, WCHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChar(LPCWSTR String, CHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChar(LPCWSTR String, WCHAR Char, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChars(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChars(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChars(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindChars(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindString(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindString(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindString(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static BOOL FindString(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true)noexcept;
	static UINT64 Hash(LPCSTR String)noexcept;
	static UINT64 Hash(LPCWSTR String)noexcept;
	static UINT Insert(LPSTR Destination, UINT Size, LPCSTR Source, UINT Position, LPCSTR Value)noexcept;
	static UINT Insert(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Position, LPCSTR Value)noexcept;
	static inline BOOL IsSet(LPCSTR String)noexcept { return String&&String[0]; }
	static inline BOOL IsSet(LPCWSTR String)noexcept { return String&&String[0]; }
	static UINT Length(LPCSTR String)noexcept;
	static UINT Length(LPCWSTR String)noexcept;
	static UINT Length(LPCSTR String, UINT Max)noexcept;
	static UINT Length(LPCWSTR String, UINT Max)noexcept;
	static UINT Length(LPCSTR Format, VariableArguments& Arguments)noexcept;
	static UINT Lowercase(LPSTR String)noexcept;
	static UINT Lowercase(LPWSTR String)noexcept;
	static UINT Lowercase(LPSTR Destination, UINT Size, LPCSTR Source)noexcept;
	static UINT Lowercase(LPWSTR Destination, UINT Size, LPCWSTR Source)noexcept;
	static UINT Print(LPSTR Destination, UINT Size, LPCSTR Format, VariableArguments& Arguments)noexcept;
	static UINT Print(LPWSTR Destination, UINT Size, LPCSTR Format, VariableArguments& Arguments)noexcept;
	template <std::character _dst_t, class... _args_t> static inline UINT Print(_dst_t* Destination, UINT Size, LPCSTR Format, _args_t... Arguments)noexcept
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Print(Destination, Size, Format, vargs);
		}
	static UINT Replace(LPSTR Destination, UINT Size, LPCSTR Source, LPCSTR Find, LPCSTR Insert, BOOL CaseSensitive, BOOL Repeat)noexcept;
	static UINT Replace(LPWSTR Destination, UINT Size, LPCWSTR Source, LPCSTR Find, LPCSTR Insert, BOOL CaseSensitive, BOOL Repeat)noexcept;
	static UINT Scan(LPCSTR String, LPCSTR Format, VariableArguments& Arguments)noexcept;
	static UINT Scan(LPCWSTR String, LPCSTR Format, VariableArguments& Arguments)noexcept;
	template <std::character _src_t, class... _args_t> static inline UINT Scan(_src_t const* String, LPCSTR Format, _args_t... Arguments)noexcept
		{
		VariableArgument args[]={ Arguments... };
		VariableArguments vargs(args, TypeHelper::ArraySize(args));
		return Scan(String, Format, vargs);
		}
	static LPCSTR Truncate(LPCSTR String, LPCSTR Characters)noexcept;
	static LPCWSTR Truncate(LPCWSTR String, LPCSTR Characters)noexcept;
	static UINT Uppercase(LPSTR String)noexcept;
	static UINT Uppercase(LPWSTR String)noexcept;
	static UINT Uppercase(LPSTR Destination, UINT Size, LPCSTR Source)noexcept;
	static UINT Uppercase(LPWSTR Destination, UINT Size, LPCWSTR Source)noexcept;
};
