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


//========
// Common
//========

UINT StringCopy(LPSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
UINT StringCopy(LPSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
UINT StringCopy(LPWSTR Destination, UINT Size, LPCSTR Source, UINT Length=0);
UINT StringCopy(LPWSTR Destination, UINT Size, LPCWSTR Source, UINT Length=0);
BOOL StringFind(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFind(LPCSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFind(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFind(LPCWSTR String, LPCWSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFindChar(LPCSTR String, CHAR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFindChar(LPCWSTR String, CHAR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFindChars(LPCSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
BOOL StringFindChars(LPCWSTR String, LPCSTR Find, UINT* Position=nullptr, BOOL CaseSensitive=true);
inline BOOL StringIsSet(LPCSTR String) { return String&&String[0]; }
inline BOOL StringIsSet(LPCWSTR String) { return String&&String[0]; }
UINT StringLength(LPCSTR String);
UINT StringLength(LPCWSTR String);
UINT StringLength(LPCSTR String, UINT Max);
UINT StringLength(LPCWSTR String, UINT Max);
LPCSTR StringTruncate(LPCSTR String, LPCSTR Chars);
LPCWSTR StringTruncate(LPCWSTR String, LPCSTR Chars);


//==============
// Modification
//==============

UINT StringAppend(LPSTR Destination, UINT Size, LPCSTR String, LPCSTR Append);
UINT StringInsert(LPSTR Destination, UINT Size, LPCSTR String, UINT Position, LPCSTR Insert);
UINT StringLowerCase(LPSTR Destination, UINT Size, LPCSTR String);
UINT StringReplace(LPSTR Destination, UINT Size, LPCSTR String, LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);
UINT StringReplace(LPWSTR Destination, UINT Size, LPCWSTR String, LPCSTR Find, LPCSTR Replace, BOOL CaseSensitive=true, BOOL Repeat=false);
UINT StringUpperCase(LPSTR Destination, UINT Size, LPCSTR String);


//============
// Formatting
//============

enum class StringFormat
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

enum class StringFormatFlags: UINT
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

UINT StringGetFormat(LPCSTR String, StringFormat& Format, StringFormatFlags& Flags, UINT& Width, UINT& Precision);
UINT StringPrint(LPSTR Buffer, UINT Size, LPCSTR String);
UINT StringPrint(LPWSTR Buffer, UINT Size, LPCSTR String);
UINT StringPrintArgs(LPSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);
UINT StringPrintArgs(LPWSTR Buffer, UINT Size, LPCSTR Format, VariableArguments const& Arguments);
UINT StringPrintDouble(LPSTR Buffer, UINT Size, DOUBLE Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0, UINT Precision=0);
UINT StringPrintFloat(LPSTR Buffer, UINT Size, FLOAT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0, UINT Precision=0);
UINT StringPrintHex(LPSTR Buffer, UINT Size, UINT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
UINT StringPrintHex64(LPSTR Buffer, UINT Size, UINT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
UINT StringPrintInt(LPSTR Buffer, UINT Size, INT Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
UINT StringPrintInt64(LPSTR Buffer, UINT Size, INT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);
UINT StringPrintUInt(LPSTR Buffer, UINT Size, UINT Value);
UINT StringPrintUInt(LPSTR Buffer, UINT Size, UINT Value, StringFormatFlags Flags, UINT Width=0);
UINT StringPrintUInt64(LPSTR Buffer, UINT Size, UINT64 Value);
UINT StringPrintUInt64(LPSTR Buffer, UINT Size, UINT64 Value, StringFormatFlags Flags=StringFormatFlags::None, UINT Width=0);

template <class _char_t, class... _args_t>
inline UINT StringPrint(_char_t* Buffer, UINT Size, LPCSTR Format, _args_t... Arguments)
{
UnknownClass args[]={ Arguments... };
VariableArguments vargs(args, ArraySize(args));
return StringPrintArgs(Buffer, Size, Format, vargs);
}


//==========
// Scanning
//==========

UINT StringScanArgs(LPCSTR String, LPCSTR Format, VariableArguments& Arguments);
UINT StringScanArgs(LPCWSTR String, LPCSTR Format, VariableArguments& Arguments);
UINT StringScanUInt(LPCSTR String, UINT* Value, UINT Base=10, UINT Length=0);
UINT StringScanUInt(LPCWSTR String, UINT* Value, UINT Base=10, UINT Length=0);
UINT StringScanUInt64(LPCSTR String, UINT64* Value, UINT Base=10, UINT Length=0);
UINT StringScanUInt64(LPCWSTR String, UINT64* Value, UINT Base=10, UINT Length=0);

template <class _char_t, class... _args_t>
inline UINT StringScan(_char_t const* String, LPCSTR Format, _args_t... Arguments)
{
UnknownClass args[]={ Arguments... };
VariableArguments vargs(args, ArraySize(args));
return StringScanArgs(String, Format, vargs);
}


//============
// Comparison
//============

INT StringCompare(LPCSTR Value1, LPCSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
INT StringCompare(LPCSTR Value1, LPCWSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
INT StringCompare(LPCWSTR Value1, LPCSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
INT StringCompare(LPCWSTR Value1, LPCWSTR Value2, UINT Length=0, BOOL CaseSensitive=true);
