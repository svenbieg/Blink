//==============
// CharHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"
#include "TypeHelper.h"


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
	static BOOL IsAlpha(CHAR Char);
	static BOOL IsAlpha(WCHAR Char);
	static BOOL IsBreak(CHAR Char);
	static BOOL IsBreak(WCHAR Char);
	static BOOL IsCapital(CHAR Char);
	static BOOL IsCapital(WCHAR Char);
	static inline BOOL IsDigit(CHAR Char, UINT Base=10) { return ToDigit(Char, nullptr, Base); }
	static inline BOOL IsDigit(WCHAR Char, UINT Base=10) { return ToDigit(Char, nullptr, Base); }
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
	static CHAR ToCapital(CHAR Char);
	static WCHAR ToCapital(WCHAR Char);
	#ifdef _UNICODE
	static inline TCHAR ToChar(CHAR Char) { return ToUnicode(Char); }
	static inline TCHAR ToChar(WCHAR Char) { return Char; }
	#else
	static inline TCHAR ToChar(CHAR Char) { return Char; }
	static inline TCHAR ToChar(WCHAR Char) { return ToAnsi(Char); }
	#endif
	static BOOL ToDigit(CHAR Char, UINT* Digit, UINT Base=10);
	static BOOL ToDigit(WCHAR Char, UINT* Digit, UINT Base=10);
	static CHAR ToSmall(CHAR Char);
	static WCHAR ToSmall(WCHAR Char);
	static WCHAR ToUnicode(CHAR Char);
	static UINT WriteAnsi(OutputStream* Stream, CHAR Char);
	static UINT WriteAnsi(OutputStream* Stream, WCHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, CHAR Char);
	static UINT WriteUnicode(OutputStream* Stream, WCHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, CHAR Char);
	static UINT WriteUtf8(OutputStream* Stream, WCHAR Char);
};
