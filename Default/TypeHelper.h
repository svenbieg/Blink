//==============
// TypeHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Exception.h"
#include "Platform.h"


//=======
// Types
//=======

typedef void VOID;

typedef decltype(nullptr) nullptr_t;

typedef unsigned char BYTE;
constexpr BYTE BYTE_MAX=0xFF;

typedef unsigned short int WORD;
constexpr WORD WORD_MAX=0xFFFF;

typedef unsigned __LONG int DWORD;
constexpr DWORD DWORD_MAX=0xFFFFFFFF;

typedef unsigned __LONG_LONG int QWORD;

typedef __ADDR_T SIZE_T;
constexpr SIZE_T SIZE_MAX=~0;

typedef int BOOL;

typedef short int SHORT;
constexpr SHORT SHORT_MAX=0x7FFF;
constexpr SHORT SHORT_MIN=-0x7FFF;

typedef int INT;
constexpr INT INT_MAX=0x7FFFFFFF;
constexpr INT INT_MIN=-0x7FFFFFFF;

typedef unsigned int UINT;
constexpr UINT UINT_MAX=(UINT)-1;
typedef UINT UINT32;

typedef __LONG_LONG int INT64;

typedef unsigned __LONG_LONG int UINT64;
constexpr UINT64 UINT64_MAX=(UINT64)-1;

typedef float FLOAT;
typedef double DOUBLE;

typedef char CHAR;
typedef wchar_t WCHAR;

typedef char* LPSTR;
typedef wchar_t* LPWSTR;

typedef char const* LPCSTR;
typedef wchar_t const* LPCWSTR;

#ifdef _UNICODE
#define _TEXT(s) L##s
#define TEXT(s) _TEXT(s)
typedef WCHAR TCHAR;
typedef LPWSTR LPTSTR;
typedef LPCWSTR LPCTSTR;
#else
#define TEXT(s) s
typedef CHAR TCHAR;
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
#endif


//========
// Macros
//========

#define _STR(s) #s
#define STR(s) _STR(s)


//=========================
// Additional Dependencies
//=========================

#include <assert.h>
#include <concepts>


//=============
// Type-Helper
//=============

class TypeHelper
{
public:
	// Common
	template <std::unsigned_integral _size_t, std::unsigned_integral _align_t> static inline _size_t AlignDown(_size_t Value, _align_t Align)
		{
		assert(Align!=0);
		return Value&~(Align-1);
		}
	template <std::unsigned_integral _size_t, std::unsigned_integral _align_t> static inline _size_t AlignUp(_size_t Value, _align_t Align)
		{
		assert(Align!=0);
		return Value+(Align-Value%Align)%Align;
		}
	template <class _item_t, SIZE_T _Count> static constexpr SIZE_T ArraySize(_item_t (&)[_Count])
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
	template <std::unsigned_integral _uint_t, std::unsigned_integral _value_t> static inline BOOL Fits(_value_t Value)
		{
		return Value<=(_uint_t)~0;
		}
	template <std::unsigned_integral _uint_t, std::signed_integral _value_t> static inline BOOL Fits(_value_t Value)
		{
		if(Value<0)
			return false;
		return Value<=(_uint_t)~0;
		}
	template <std::signed_integral _int_t, std::unsigned_integral _value_t> static inline BOOL Fits(_value_t Value)
		{
		using uint_t=typename std::make_unsigned<_int_t>::type;
		_int_t max=((uint_t)~0)/2;
		_int_t min=-max;
		using value_t=typename std::make_signed<_value_t>::type;
		value_t value=Value;
		return (value>=min)&&(value<=max);
		}
	template <std::signed_integral _int_t, std::signed_integral _value_t> static inline BOOL Fits(_value_t Value)
		{
		using uint_t=typename std::make_unsigned<_int_t>::type;
		_int_t max=((uint_t)~0)/2;
		_int_t min=-max;
		return (Value>=min)&&(Value<=max);
		}
	static inline UINT LowLong(UINT64 Value) { return (UINT)Value; }
	static inline WORD MakeLong(BYTE Low, BYTE High)
		{
		return (((WORD)High)<<8)|Low;
		}
	static inline UINT MakeLong(BYTE Low, BYTE High8, BYTE High16, BYTE High24)
		{
		return (((UINT)High24)<<24)|((UINT)High16<<16)|((UINT)High8<<8)|Low;
		}
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
	template <std::signed_integral _int_t, std::integral _value_t> static _int_t Signed(_value_t Value)
		{
		using int_t=typename std::make_signed<_value_t>::type;
		int_t value=Value;
		if(!Fits<_int_t, int_t>(value))
			throw InvalidArgumentException();
		return static_cast<_int_t>(value);
		}
	template <std::signed_integral _int_t, std::integral _value_t> static bool Signed(_value_t Value, _int_t* Signed)
		{
		using int_t=typename std::make_signed<_value_t>::type;
		int_t value=Value;
		if(!Fits<_int_t, int_t>(value))
			return false;
		*Signed=static_cast<_int_t>(value);
		return true;
		}
	template <std::unsigned_integral _uint_t, std::integral _value_t> static _uint_t Unsigned(_value_t Value)
		{
		using uint_t=typename std::make_unsigned<_value_t>::type;
		uint_t value=Value;
		if(!Fits<_uint_t, uint_t>(value))
			throw InvalidArgumentException();
		return static_cast<_uint_t>(value);
		}
	template <std::unsigned_integral _uint_t, std::integral _value_t> static bool Unsigned(_value_t Value, _uint_t* Unsigned)
		{
		using uint_t=typename std::make_unsigned<_value_t>::type;
		uint_t value=Value;
		if(!Fits<_uint_t, uint_t>(value))
			return false;
		*Unsigned=static_cast<_uint_t>(value);
		return true;
		}
};
