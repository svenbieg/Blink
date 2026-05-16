//==============
// CharHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"


//=========
// Concept
//=========

namespace std
{
template <class _char_t> struct is_character { static constexpr bool value=false; };
template <> struct is_character<CHAR> { static constexpr bool value=true; };
template <> struct is_character<WCHAR> { static constexpr bool value=true; };
template<class _Ty> concept character=is_character<_Ty>::value;
}


//=============
// Char-Helper
//=============

class CharHelper
{
public:
	// Using
	using InputStream=Storage::Streams::InputStream;
	using OutputStream=Storage::Streams::OutputStream;

	// Common
	static INT Compare(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true)noexcept;
	static INT Compare(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true)noexcept;
	static INT Compare(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true)noexcept;
	static INT Compare(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(CHAR Char1, LPCSTR Chars, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(WCHAR Char1, LPCSTR Chars, BOOL CaseSensitive=true)noexcept;
	static BOOL Equal(WCHAR Char1, LPCWSTR Chars, BOOL CaseSensitive=true)noexcept;
	static BOOL IsAlpha(CHAR Char)noexcept;
	static BOOL IsAlpha(WCHAR Char)noexcept;
	static BOOL IsBreak(CHAR Char)noexcept;
	static BOOL IsBreak(WCHAR Char)noexcept;
	static BOOL IsCapital(CHAR Char)noexcept;
	static BOOL IsCapital(WCHAR Char)noexcept;
	static inline BOOL IsDigit(CHAR Char, UINT Base=10)noexcept { return ToDigit(Char, nullptr, Base); }
	static inline BOOL IsDigit(WCHAR Char, UINT Base=10)noexcept { return ToDigit(Char, nullptr, Base); }
	static BOOL IsLineBreak(CHAR Char)noexcept;
	static BOOL IsLineBreak(WCHAR Char)noexcept;
	static BOOL IsPrintable(CHAR Char)noexcept;
	static BOOL IsPrintable(WCHAR Char)noexcept;
	static BOOL IsSmall(CHAR Char)noexcept;
	static BOOL IsSmall(WCHAR Char)noexcept;
	static BOOL IsSpecial(CHAR Char)noexcept;
	static BOOL IsSpecial(WCHAR Char)noexcept;
	static UINT Read(LPCSTR String);
	static UINT Read(LPCSTR String, CHAR* Char);
	static UINT Read(LPCSTR String, WCHAR* Char);
	static UINT Read(LPCWSTR String);
	static UINT Read(LPCWSTR String, CHAR* Char);
	static UINT Read(LPCWSTR String, WCHAR* Char);
	static UINT Read(InputStream* Stream);
	static UINT Read(InputStream* Stream, CHAR* Char);
	static UINT Read(InputStream* Stream, WCHAR* Char);
	static CHAR ToAnsi(WCHAR Char)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToCapital(_src_t Char)noexcept;
	static CHAR ToCapitalAnsi(CHAR Char)noexcept;
	static CHAR ToCapitalAnsi(WCHAR Char)noexcept;
	static WCHAR ToCapitalUnicode(CHAR Char)noexcept;
	static WCHAR ToCapitalUnicode(WCHAR Char)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToChar(_src_t Char)noexcept;
	static BOOL ToDigit(CHAR Char, UINT* Digit, UINT Base=10)noexcept;
	static BOOL ToDigit(WCHAR Char, UINT* Digit, UINT Base=10)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToSmall(_src_t Char)noexcept;
	static CHAR ToSmallAnsi(CHAR Char)noexcept;
	static CHAR ToSmallAnsi(WCHAR Char)noexcept;
	static WCHAR ToSmallUnicode(CHAR Char)noexcept;
	static WCHAR ToSmallUnicode(WCHAR Char)noexcept;
	static WCHAR ToUnicode(CHAR Char)noexcept;
	static UINT Write(LPSTR Buffer, UINT Size, CHAR Char);
	static UINT Write(LPSTR Buffer, UINT Size, WCHAR Char);
	static UINT Write(LPWSTR Buffer, UINT Size, CHAR Char);
	static UINT Write(LPWSTR Buffer, UINT Size, WCHAR Char);
	static UINT Write(OutputStream* Stream, CHAR Char);
	static UINT Write(OutputStream* Stream, WCHAR Char);
};

template <> inline CHAR CharHelper::ToCapital(CHAR Char)noexcept { return ToCapitalAnsi(Char); }
template <> inline CHAR CharHelper::ToCapital(WCHAR Char)noexcept { return ToCapitalAnsi(Char); }
template <> inline WCHAR CharHelper::ToCapital(CHAR Char)noexcept { return ToCapitalUnicode(Char); }
template <> inline WCHAR CharHelper::ToCapital(WCHAR Char)noexcept { return ToCapitalUnicode(Char); }

template <> inline CHAR CharHelper::ToChar(CHAR Char)noexcept {return Char; }
template <> inline CHAR CharHelper::ToChar(WCHAR Char)noexcept { return ToAnsi(Char); }
template <> inline WCHAR CharHelper::ToChar(CHAR Char)noexcept { return ToUnicode(Char); }
template <> inline WCHAR CharHelper::ToChar(WCHAR Char)noexcept { return Char; }

template <> inline CHAR CharHelper::ToSmall(CHAR Char)noexcept { return ToSmallAnsi(Char); }
template <> inline CHAR CharHelper::ToSmall(WCHAR Char)noexcept { return ToSmallAnsi(Char); }
template <> inline WCHAR CharHelper::ToSmall(CHAR Char)noexcept { return ToSmallUnicode(Char); }
template <> inline WCHAR CharHelper::ToSmall(WCHAR Char)noexcept { return ToSmallUnicode(Char); }
