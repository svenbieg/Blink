//==============
// CharHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"


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


//============
// Characters
//============

namespace Ansi
{
constexpr CHAR AE='\xC4';
constexpr CHAR ae='\xE4';
constexpr CHAR Average='\xD8';
constexpr CHAR Degree='\xB0';
constexpr CHAR Enumeration='\x95';
constexpr CHAR OE='\xD6';
constexpr CHAR oe='\xF6';
constexpr CHAR UE='\xDC';
constexpr CHAR ue='\xFC';
}

#ifdef _UNICODE
namespace Chars
{
constexpr WCHAR AE=L'\x00C4';
constexpr WCHAR ae=L'\x00E4';
constexpr WCHAR Average=L'\x00D8';
constexpr WCHAR Degree=L'\x00B0';
constexpr WCHAR Enumeration=L'\x2022';
constexpr WCHAR OE=L'\x00D6';
constexpr WCHAR oe=L'\x00F6';
constexpr WCHAR UE=L'\x00DC';
constexpr WCHAR ue=L'\x00FC';
}
#else
namespace Chars
{
constexpr CHAR AE='\xC4';
constexpr CHAR ae='\xE4';
constexpr CHAR Average='\xD8';
constexpr CHAR Degree='\xB0';
constexpr CHAR Enumeration='\x95';
constexpr CHAR OE='\xD6';
constexpr CHAR oe='\xF6';
constexpr CHAR UE='\xDC';
constexpr CHAR ue='\xFC';
}
#endif

namespace Unicode
{
constexpr WCHAR AE=L'\x00C4';
constexpr WCHAR ae=L'\x00E4';
constexpr WCHAR Average=L'\x00D8';
constexpr WCHAR Degree=L'\x00B0';
constexpr WCHAR Enumeration=L'\x2022';
constexpr WCHAR OE=L'\x00D6';
constexpr WCHAR oe=L'\x00F6';
constexpr WCHAR UE=L'\x00DC';
constexpr WCHAR ue=L'\x00FC';
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
	static INT Compare(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
	static INT Compare(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
	static INT Compare(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
	static INT Compare(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
	static BOOL Equal(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
	static BOOL Equal(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
	static BOOL Equal(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
	static BOOL Equal(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
	static BOOL Equal(CHAR Char1, LPCSTR Chars, BOOL CaseSensitive=true);
	static BOOL Equal(WCHAR Char1, LPCSTR Chars, BOOL CaseSensitive=true);
	static BOOL Equal(WCHAR Char1, LPCWSTR Chars, BOOL CaseSensitive=true);
	static BOOL IsAlpha(CHAR Char);
	static BOOL IsAlpha(WCHAR Char);
	static BOOL IsBreak(CHAR Char);
	static BOOL IsBreak(WCHAR Char);
	static BOOL IsCapital(CHAR Char);
	static BOOL IsCapital(WCHAR Char);
	static inline BOOL IsDigit(CHAR Char, UINT Base=10) { return ToDigit(Char, nullptr, Base); }
	static inline BOOL IsDigit(WCHAR Char, UINT Base=10) { return ToDigit(Char, nullptr, Base); }
	static BOOL IsLineBreak(CHAR Char);
	static BOOL IsLineBreak(WCHAR Char);
	static BOOL IsPrintable(CHAR Char);
	static BOOL IsPrintable(WCHAR Char);
	static BOOL IsSmall(CHAR Char);
	static BOOL IsSmall(WCHAR Char);
	static BOOL IsSpecial(CHAR Char);
	static BOOL IsSpecial(WCHAR Char);
	static UINT ReadAnsi(InputStream* Stream, CHAR* Char);
	static UINT ReadAnsi(InputStream* Stream, WCHAR* Char);
	static UINT ReadUnicode(InputStream* Stream, CHAR* Char);
	static UINT ReadUnicode(InputStream* Stream, WCHAR* Char);
	static UINT ReadUtf8(InputStream* Stream, CHAR* Char);
	static UINT ReadUtf8(InputStream* Stream, WCHAR* Char);
	static CHAR ToAnsi(WCHAR Char);
	template <class _dst_t, class _src_t> static _dst_t ToCapital(_src_t Char);
	static CHAR ToCapitalAnsi(CHAR Char);
	static CHAR ToCapitalAnsi(WCHAR Char);
	static WCHAR ToCapitalUnicode(CHAR Char);
	static WCHAR ToCapitalUnicode(WCHAR Char);
	template <class _dst_t, class _src_t> static _dst_t ToChar(_src_t Char);
	static BOOL ToDigit(CHAR Char, UINT* Digit, UINT Base=10);
	static BOOL ToDigit(WCHAR Char, UINT* Digit, UINT Base=10);
	template <class _dst_t, class _src_t> static _dst_t ToSmall(_src_t Char);
	static CHAR ToSmallAnsi(CHAR Char);
	static CHAR ToSmallAnsi(WCHAR Char);
	static WCHAR ToSmallUnicode(CHAR Char);
	static WCHAR ToSmallUnicode(WCHAR Char);
	static WCHAR ToUnicode(CHAR Char);
	static UINT WriteAnsi(OutputStream* Stream, CHAR Char);
	static UINT WriteAnsi(OutputStream* Stream, WCHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, CHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, WCHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, CHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, WCHAR Char);
};

template <> inline CHAR CharHelper::ToCapital(CHAR Char) { return ToCapitalAnsi(Char); }
template <> inline CHAR CharHelper::ToCapital(WCHAR Char) { return ToCapitalAnsi(Char); }
template <> inline WCHAR CharHelper::ToCapital(CHAR Char) { return ToCapitalUnicode(Char); }
template <> inline WCHAR CharHelper::ToCapital(WCHAR Char) { return ToCapitalUnicode(Char); }

template <> inline CHAR CharHelper::ToChar(CHAR Char) {return Char; }
template <> inline CHAR CharHelper::ToChar(WCHAR Char) { return ToAnsi(Char); }
template <> inline WCHAR CharHelper::ToChar(CHAR Char) { return ToUnicode(Char); }
template <> inline WCHAR CharHelper::ToChar(WCHAR Char) { return Char; }

template <> inline CHAR CharHelper::ToSmall(CHAR Char) { return ToSmallAnsi(Char); }
template <> inline CHAR CharHelper::ToSmall(WCHAR Char) { return ToSmallAnsi(Char); }
template <> inline WCHAR CharHelper::ToSmall(CHAR Char) { return ToSmallUnicode(Char); }
template <> inline WCHAR CharHelper::ToSmall(WCHAR Char) { return ToSmallUnicode(Char); }
