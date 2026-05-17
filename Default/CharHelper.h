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
// Comparison
//============

enum class CompareMode
{
IgnoreCase
};


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
	static INT Compare(CHAR Char1, CHAR Char2)noexcept;
	static INT Compare(CHAR Char1, CHAR Char2, CompareMode Mode)noexcept;
	static INT Compare(CHAR Char1, WCHAR Char2)noexcept;
	static INT Compare(CHAR Char1, WCHAR Char2, CompareMode Mode)noexcept;
	static INT Compare(WCHAR Char1, CHAR Char2)noexcept;
	static INT Compare(WCHAR Char1, CHAR Char2, CompareMode Mode)noexcept;
	static INT Compare(WCHAR Char1, WCHAR Char2)noexcept;
	static INT Compare(WCHAR Char1, WCHAR Char2, CompareMode Mode)noexcept;
	static inline BOOL Equal(CHAR Char1, CHAR Char2)noexcept { return Char1==Char2; }
	static BOOL Equal(CHAR Char1, CHAR Char2, CompareMode Mode)noexcept;
	static BOOL Equal(CHAR Char1, WCHAR Char2)noexcept;
	static BOOL Equal(CHAR Char1, WCHAR Char2, CompareMode Mode)noexcept;
	static BOOL Equal(WCHAR Char1, CHAR Char2)noexcept;
	static BOOL Equal(WCHAR Char1, CHAR Char2, CompareMode Mode)noexcept;
	static inline BOOL Equal(WCHAR Char1, WCHAR Char2)noexcept { return Char1==Char2; }
	static BOOL Equal(WCHAR Char1, WCHAR Char2, CompareMode Mode)noexcept;
	static BOOL Equal(CHAR Char, LPCSTR Chars)noexcept;
	static BOOL Equal(CHAR Char, LPCSTR Chars, CompareMode Mode)noexcept;
	static BOOL Equal(WCHAR Char, LPCSTR Chars)noexcept;
	static BOOL Equal(WCHAR Char, LPCSTR Chars, CompareMode Mode)noexcept;
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
	static inline  UINT Read(LPCSTR String) { return Read(String, (WCHAR*)nullptr); }
	static UINT Read(LPCSTR String, CHAR* Char);
	static UINT Read(LPCSTR String, WCHAR* Char);
	static inline  UINT Read(LPCWSTR String) { return 1; }
	static UINT Read(LPCWSTR String, CHAR* Char);
	static UINT Read(LPCWSTR String, WCHAR* Char);
	static inline  UINT Read(InputStream* Stream) { return Read(Stream, (WCHAR*)nullptr); }
	static UINT Read(InputStream* Stream, CHAR* Char);
	static UINT Read(InputStream* Stream, WCHAR* Char);
	static CHAR ToCapital(CHAR Char)noexcept;
	static CHAR ToCapital(WCHAR Char)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToCapitalT(_src_t Char)noexcept;
	static WCHAR ToCapitalW(CHAR Char)noexcept;
	static WCHAR ToCapitalW(WCHAR Char)noexcept;
	static inline CHAR ToChar(CHAR Char)noexcept { return Char; }
	static CHAR ToChar(WCHAR Char)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToCharT(_src_t Char)noexcept;
	static WCHAR ToCharW(CHAR Char)noexcept;
	static inline  WCHAR ToCharW(WCHAR Char)noexcept { return Char; }
	static BOOL ToDigit(CHAR Char, UINT* Digit, UINT Base=10)noexcept;
	static BOOL ToDigit(WCHAR Char, UINT* Digit, UINT Base=10)noexcept;
	static CHAR ToSmall(CHAR Char)noexcept;
	static CHAR ToSmall(WCHAR Char)noexcept;
	template <std::character _dst_t, std::character _src_t> static _dst_t ToSmallT(_src_t Char)noexcept;
	static WCHAR ToSmallW(CHAR Char)noexcept;
	static WCHAR ToSmallW(WCHAR Char)noexcept;
	static UINT Write(LPSTR Buffer, UINT Size, CHAR Char);
	static UINT Write(LPSTR Buffer, UINT Size, WCHAR Char);
	static UINT Write(LPWSTR Buffer, UINT Size, CHAR Char);
	static UINT Write(LPWSTR Buffer, UINT Size, WCHAR Char);
	static UINT Write(OutputStream* Stream, CHAR Char);
	static UINT Write(OutputStream* Stream, WCHAR Char);
};

template <> inline  CHAR CharHelper::ToCapitalT(CHAR Char)noexcept { return ToCapital(Char); }
template <> inline  CHAR CharHelper::ToCapitalT(WCHAR Char)noexcept { return ToCapital(Char); }
template <> inline  WCHAR CharHelper::ToCapitalT(CHAR Char)noexcept { return ToCapitalW(Char); }
template <> inline  WCHAR CharHelper::ToCapitalT(WCHAR Char)noexcept { return ToCapitalW(Char); }

template <> inline  CHAR CharHelper::ToCharT(CHAR Char)noexcept { return Char; }
template <> inline  CHAR CharHelper::ToCharT(WCHAR Char)noexcept { return ToChar(Char); }
template <> inline  WCHAR CharHelper::ToCharT(CHAR Char)noexcept { return ToCharW(Char); }
template <> inline  WCHAR CharHelper::ToCharT(WCHAR Char)noexcept { return Char; }

template <> inline  CHAR CharHelper::ToSmallT(CHAR Char)noexcept { return ToSmall(Char); }
template <> inline  CHAR CharHelper::ToSmallT(WCHAR Char)noexcept { return ToSmall(Char); }
template <> inline  WCHAR CharHelper::ToSmallT(CHAR Char)noexcept { return ToSmallW(Char); }
template <> inline  WCHAR CharHelper::ToSmallT(WCHAR Char)noexcept { return ToSmallW(Char); }
