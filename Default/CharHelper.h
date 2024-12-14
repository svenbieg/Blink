//==============
// CharHelper.h
//==============

#pragma once


//============
// Characters
//============

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


//============
// Conversion
//============

CHAR CharToAnsi(WCHAR Char);
CHAR CharToCapital(CHAR Char);
WCHAR CharToCapital(WCHAR Char);
BOOL CharToDigit(CHAR Char, UINT* Digit, UINT Base=10);
BOOL CharToDigit(WCHAR Char, UINT* Digit, UINT Base=10);
BYTE CharToHex(CHAR Char);
BYTE CharToHex(WCHAR Char);
CHAR CharToSmall(CHAR Char);
WCHAR CharToSmall(WCHAR Char);
WCHAR CharToUnicode(CHAR Char);

template <class _convert_t, class _char_t> inline _convert_t CharToChar(_char_t Char)
{
return (_convert_t)Char;
}

template <> inline CHAR CharToChar(WCHAR Char)
{
return CharToAnsi(Char);
}

template <> inline WCHAR CharToChar(CHAR Char)
{
return CharToUnicode(Char);
}


//========
// Common
//========

BOOL CharIsAlpha(CHAR Char);
BOOL CharIsAlpha(WCHAR Char);
BOOL CharIsBreak(CHAR Char);
BOOL CharIsBreak(WCHAR Char);
BOOL CharIsCapital(CHAR Char);
BOOL CharIsCapital(WCHAR Char);
BOOL CharIsDigit(CHAR Char, UINT Base=10);
BOOL CharIsDigit(WCHAR Char, UINT Base=10);
BOOL CharIsPrintable(CHAR Char);
BOOL CharIsPrintable(WCHAR Char);
BOOL CharIsSmall(CHAR Char);
BOOL CharIsSmall(WCHAR Char);
BOOL CharIsSpecial(CHAR Char);
BOOL CharIsSpecial(WCHAR Char);


//============
// Comparison
//============

INT CharCompare(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
INT CharCompare(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
INT CharCompare(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
INT CharCompare(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true);

template <class _char1_t, class _char2_t> inline BOOL CharEqual(_char1_t Char1, _char2_t Char2)
{
CHAR c1=CharToChar<CHAR, _char1_t>(Char1);
CHAR c2=CharToChar<CHAR, _char2_t>(Char2);
return c1==c2;
}

BOOL CharEqual(CHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
BOOL CharEqual(CHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
BOOL CharEqual(WCHAR Char1, CHAR Char2, BOOL CaseSensitive=true);
BOOL CharEqual(WCHAR Char1, WCHAR Char2, BOOL CaseSensitive=true);
