//==============
// TypeHelper.h
//==============

#pragma once


//=======
// Using
//=======

#include "Exception.h"
#include <bit>
#include <cassert>
#include <climits>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <limits>


//=======
// Types
//=======

typedef void VOID;
typedef decltype(nullptr) nullptr_t;

typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef unsigned __LONG int DWORD;
typedef unsigned __LONG_LONG int QWORD;
typedef __ADDR_T SIZE_T;

typedef int BOOL;
typedef short int SHORT;
typedef int INT;
typedef unsigned int UINT;
typedef UINT UINT32;
typedef __LONG_LONG int INT64;
typedef unsigned __LONG_LONG int UINT64;

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


//==========
// Concepts
//==========

namespace std
{
template <class _Ty> concept numeric=is_integral_v<_Ty>||is_floating_point_v<_Ty>;
}


//=============
// Type-Helper
//=============

class TypeHelper
{
public:
	// Common
	template <std::unsigned_integral _size_t, std::unsigned_integral _align_t> static inline _size_t AlignDown(_size_t Value, _align_t Align)noexcept
		{
		assert(Align!=0);
		return Value&~(Align-1);
		}
	template <std::unsigned_integral _size_t, std::unsigned_integral _align_t> static inline _size_t AlignUp(_size_t Value, _align_t Align)noexcept
		{
		assert(Align!=0);
		return Value+(Align-Value%Align)%Align;
		}
	template <class _item_t, SIZE_T _count> static constexpr SIZE_T ArraySize(_item_t (&)[_count])noexcept { return _count; }
	template <std::unsigned_integral _value_t> static constexpr _value_t BigEndian(_value_t Value)noexcept
		{
		if constexpr(std::endian::native==std::endian::little)
			return std::byteswap(Value);
		return Value;
		}
	static constexpr UINT HighLong(UINT64 Value)noexcept { return (UINT)(Value>>32); }
	template <std::numeric _num_t, std::numeric _value_t> static constexpr BOOL Fits(_value_t Value)noexcept
		{
		constexpr auto min=std::numeric_limits<_num_t>::min();
		constexpr auto max=std::numeric_limits<_num_t>::max();
		return std::isgreaterequal(Value, min)&&std::islessequal(Value, max);
		}
	template <std::floating_point _float_t, std::numeric _value_t> static constexpr _float_t Float(_value_t Value)
		{
		if(!Fits<_float_t, _value_t>(Value))
			throw InvalidArgumentException();
		return static_cast<_float_t>(Value);
		}
	template <std::floating_point _float_t, std::numeric _value_t> static constexpr BOOL Float(_value_t Value, _float_t* Result)noexcept
		{
		if(!Fits<_float_t, _value_t>(Value))
			return false;
		*Result=static_cast<_float_t>(Value);
		return true;
		}
	template <std::unsigned_integral _value_t> static constexpr _value_t LittleEndian(_value_t Value)noexcept
		{
		if constexpr(std::endian::native==std::endian::big)
			return std::byteswap(Value);
		return Value;
		}
	static constexpr UINT LowLong(UINT64 Value)noexcept { return (UINT)Value; }
	static constexpr WORD MakeLong(BYTE Low, BYTE High)noexcept
		{
		return (((WORD)High)<<8)|Low;
		}
	static constexpr UINT MakeLong(BYTE Low, BYTE High8, BYTE High16, BYTE High24)noexcept
		{
		return (((UINT)High24)<<24)|((UINT)High16<<16)|((UINT)High8<<8)|Low;
		}
	static constexpr UINT MakeLong(WORD Low, WORD High)noexcept
		{
		return (((UINT)High)<<16)|Low;
		}
	static constexpr UINT64 MakeLong(UINT Low, UINT High)noexcept
		{
		return (((UINT64)High)<<32)|Low;
		}
	template <class _size1_t, class _size2_t> static constexpr _size1_t Max(_size1_t Value1, _size2_t Value2)noexcept
		{
		if(Value1>Value2)
			return Value1;
		return Value2;
		}
	template <class _size1_t, class _size2_t> static constexpr _size1_t Min(_size1_t Value1, _size2_t Value2)noexcept
		{
		if(Value1<Value2)
			return Value1;
		return Value2;
		}
	template <std::integral _int_t, std::integral _value_t> static constexpr _int_t Integral(_value_t Value)
		{
		if(!Fits<_int_t, _value_t>(Value))
			throw InvalidArgumentException();
		return static_cast<_int_t>(Value);
		}
	template <std::integral _int_t, std::floating_point _value_t> static constexpr _int_t Integral(_value_t Value)
		{
		return Integral<_int_t, long long>(std::llround(Value));
		}
	template <std::integral _int_t, std::integral _value_t> static constexpr bool Integral(_value_t Value, _int_t* Result)noexcept
		{
		if(!Fits<_int_t, _value_t>(Value))
			return false;
		*Result=static_cast<_int_t>(Value);
		return true;
		}
	template <std::integral _int_t, std::floating_point _value_t> static constexpr bool Integral(_value_t Value, _int_t* Result)noexcept
		{
		return Integral<_int_t, long long>(std::llround(Value), Result);
		}
};
