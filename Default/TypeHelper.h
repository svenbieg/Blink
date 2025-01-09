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
constexpr WORD WORD_MAX=0xFFFF;

typedef unsigned __long int DWORD;
typedef unsigned __long_long int QWORD;

typedef int BOOL;

typedef short int SHORT;
typedef unsigned short int USHORT;

typedef int INT;
constexpr INT INT_MAX=0x7FFFFFFF;
constexpr INT INT_MIN=-0x7FFFFFFF;

typedef unsigned int UINT;
constexpr UINT UINT_MAX=0xFFFFFFFF;

typedef __long_long int INT64;

typedef unsigned __long_long int UINT64;
constexpr UINT64 UINT64_MAX=0xFFFFFFFFFFFFFFFFULL;

typedef float FLOAT;
typedef double DOUBLE;

typedef char CHAR;
typedef wchar_t WCHAR;

typedef char* LPSTR;
typedef wchar_t* LPWSTR;

typedef char const* LPCSTR;
typedef wchar_t const* LPCWSTR;

#undef TEXT
#ifdef _UNICODE
#define _TEXT(s) L#s
typedef WCHAR TCHAR;
typedef LPWSTR LPTSTR;
typedef LPCWSTR LPCTSTR;
#else
#define _TEXT(s) #s
typedef CHAR TCHAR;
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
#endif
#define TEXT(s) _TEXT(s)


//========
// Macros
//========

#define _STR(s) #s
#define STR(s) _STR(s)


//=============
// Type-Helper
//=============

class TypeHelper
{
public:
	// Common
	template <class _size_t> static inline _size_t AlignDown(_size_t Value, UINT Align)
		{
		return Value&~(Align-1);
		}
	template <class _size_t> static inline _size_t AlignUp(_size_t Value, UINT Align)
		{
		return Value+(Align-Value%Align)%Align;
		}
	template <class _item_t, UINT _Count> static constexpr UINT ArraySize(_item_t (&)[_Count])
		{
		return _Count;
		}
	static inline WORD BigEndian(WORD Value)
		{
		return (Value>>8)|(Value<<8);
		}
	static inline UINT BigEndian(UINT Value)
		{
		UINT value=(Value>>24);
		value|=(Value>>8)&0xFF00;
		value|=(Value<<8)&0xFF0000;
		value|=(Value<<24);
		return value;
		}
	static inline UINT HighLong(UINT64 Value) { return (UINT)(Value>>32); }
	static inline UINT LowLong(UINT64 Value) { return (UINT)Value; }
	static inline UINT MakeLong(WORD Low, WORD High)
		{
		return (((UINT)High)<<16)|Low;
		}
	static inline UINT64 MakeLong(UINT Low, UINT High)
		{
		return (((UINT64)High)<<32)|Low;
		}
	template <class _size1_t, class _size2_t> static inline _size1_t Max(_size1_t Value1, _size2_t Value2)
		{
		if(Value1>Value2)
			return Value1;
		return Value2;
		}
	template <class _size1_t, class _size2_t> static inline _size1_t Min(_size1_t Value1, _size2_t Value2)
		{
		if(Value1<Value2)
			return Value1;
		return Value2;
		}
};
