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


//============
// Characters
//============

namespace Ansi
{
const CHAR AE='\xC4';
const CHAR ae='\xE4';
const CHAR Average='\xD8';
const CHAR Degree='\xB0';
const CHAR Enumeration='\x95';
const CHAR OE='\xD6';
const CHAR oe='\xF6';
const CHAR sz='\xDF';
const CHAR UE='\xDC';
const CHAR ue='\xFC';
}

#ifdef _UNICODE
namespace Chars
{
const WCHAR AE=L'\x00C4';
const WCHAR ae=L'\x00E4';
const WCHAR Average=L'\x00D8';
const WCHAR Degree=L'\x00B0';
const WCHAR Enumeration=L'\x2022';
const WCHAR OE=L'\x00D6';
const WCHAR oe=L'\x00F6';
const WCHAR UE=L'\x00DC';
const WCHAR ue=L'\x00FC';
}
#else
namespace Chars
{
const CHAR AE='\xC4';
const CHAR ae='\xE4';
const CHAR Average='\xD8';
const CHAR Degree='\xB0';
const CHAR Enumeration='\x95';
const CHAR OE='\xD6';
const CHAR oe='\xF6';
const CHAR UE='\xDC';
const CHAR ue='\xFC';
}
#endif

namespace Unicode
{
const WCHAR AE=L'\x00C4';
const WCHAR ae=L'\x00E4';
const WCHAR Average=L'\x00D8';
const WCHAR Degree=L'\x00B0';
const WCHAR Enumeration=L'\x2022';
const WCHAR OE=L'\x00D6';
const WCHAR oe=L'\x00F6';
const WCHAR UE=L'\x00DC';
const WCHAR ue=L'\x00FC';
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
	static UINT ReadAnsi(InputStream* Stream, CHAR* Char);
	static UINT ReadAnsi(InputStream* Stream, WCHAR* Char);
	static UINT ReadUnicode(InputStream* Stream, CHAR* Char);
	static UINT ReadUnicode(InputStream* Stream, WCHAR* Char);
	static UINT ReadUtf8(InputStream* Stream, CHAR* Char);
	static UINT ReadUtf8(InputStream* Stream, WCHAR* Char);
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
	static UINT WriteAnsi(OutputStream* Stream, CHAR Char);
	static UINT WriteAnsi(OutputStream* Stream, WCHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, CHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, WCHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, CHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, WCHAR Char);
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
