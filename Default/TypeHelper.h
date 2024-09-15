//==============
// TypeHelper.h
//==============

#pragma once


//========
// Common
//========

typedef void VOID;

typedef decltype(nullptr) nullptr_t;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned long int DWORD;
typedef long long unsigned int QWORD;

typedef int BOOL;

typedef short int SHORT;
typedef unsigned short int USHORT;

typedef int INT;
constexpr INT INT_MAX=0x7FFFFFFF;
constexpr INT INT_MIN=-0x7FFFFFFF;

typedef unsigned int UINT;
constexpr UINT UINT_MAX=0xFFFFFFFF;

typedef long long int INT64;
typedef long long unsigned int UINT64;

typedef float FLOAT;
typedef double DOUBLE;

typedef char CHAR;
typedef wchar_t WCHAR;

typedef char* LPSTR;
typedef wchar_t* LPWSTR;

typedef char const* LPCSTR;
typedef wchar_t const* LPCWSTR;

#ifdef _UNICODE
typedef WCHAR TCHAR;
typedef LPWSTR LPTSTR;
typedef LPCWSTR LPCTSTR;
#else
typedef CHAR TCHAR;
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
#endif

typedef VOID (*PROCEDURE)();


//============
// Conversion
//============

inline UINT MakeLong(WORD Low, WORD High)
{
return (((UINT)High)<<16)|Low;
}

inline UINT64 MakeLong(UINT Low, UINT High)
{
return (((UINT64)High)<<32)|Low;
}
