//==============
// TypeHelper.h
//==============

#pragma once


//=======
// Types
//=======

typedef void VOID;

typedef decltype(nullptr) nullptr_t;

typedef unsigned char BYTE;
typedef unsigned short int WORD;

typedef int BOOL;

typedef short int SHORT;
typedef unsigned short int USHORT;

typedef int INT;
constexpr INT INT_MAX=0x7FFFFFFF;
constexpr INT INT_MIN=-0x7FFFFFFF;

typedef unsigned int UINT;
constexpr UINT UINT_MAX=0xFFFFFFFF;

typedef long int LONG;

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


//========
// Common
//========

#define __str(s) #s
#define STR(s) __str(s)

template <class _item_t, UINT _Count> constexpr UINT ArraySize(_item_t (&)[_Count])
{
return _Count;
}

template <class _size1_t, class _size2_t> inline _size1_t Max(_size1_t Value1, _size2_t Value2)
{
if(Value1>Value2)
	return Value1;
return Value2;
}

template <class _size1_t, class _size2_t> inline _size1_t Min(_size1_t Value1, _size2_t Value2)
{
if(Value1<Value2)
	return Value1;
return Value2;
}


//======
// Long
//======

inline UINT HighLong(UINT64 Value) { return (UINT)(Value>>32); }
inline UINT LowLong(UINT64 Value) { return (UINT)Value; }

inline UINT MakeLong(WORD Low, WORD High)
{
return (((UINT)High)<<16)|Low;
}

inline UINT64 MakeLong(UINT Low, UINT High)
{
return (((UINT64)High)<<32)|Low;
}


//============
// Big Endian
//============

template <class _value_t> inline _value_t BigEndian(_value_t Value)
{
static_assert(false);
}

template <> inline WORD BigEndian(WORD Value)
{
return (Value>>8)|(Value<<8);
}

template <> inline UINT BigEndian(UINT Value)
{
UINT value=(Value>>24);
value|=(Value>>8)&0xFF00;
value|=(Value<<8)&0xFF0000;
value|=(Value<<24);
return value;
}
