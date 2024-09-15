//================
// MemoryHelper.h
//================

#pragma once


//========
// Macros
//========

#define ALIGN(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))

#define _STR(s) #s
#define STR(s) _STR(s)

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

inline UINT HighLong(UINT64 Value) { return (UINT)(Value>>32); }
inline UINT LowLong(UINT64 Value) { return (UINT)Value; }


//========
// Common
//========

INT CompareMemory(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size);
VOID CopyMemory(VOID* Destination, VOID const* Source, SIZE_T Size);
VOID FillMemory(VOID* Destination, SIZE_T Size, UINT Value);
VOID MoveMemory(VOID* Destination, VOID const* Source, SIZE_T Size);
VOID ZeroMemory(VOID* Destination, SIZE_T Size);


//===========
// Alignment
//===========

template <class _size_t>
inline _size_t AlignDown(_size_t Value, UINT Align)
{
return Value&~(Align-1);
}

template <class _size_t>
inline _size_t AlignUp(_size_t Value, UINT Align)
{
return Value+(Align-Value%Align)%Align;
}


//===============
// Little Endian
//===============

template <class _value_t> inline _value_t Reverse(_value_t Value)
{
auto value_ptr=(BYTE*)&Value;
_value_t retval=0;
auto retval_ptr=(BYTE*)&retval;
for(UINT u=0; u<sizeof(_value_t); u++)
	retval_ptr[u]=value_ptr[sizeof(_value_t)-u-1];
return retval;
}

inline VOID ReverseCopy(VOID* Destination, VOID const* Source, SIZE_T Size)
{
auto dst=(BYTE*)Destination;
auto src=(BYTE const*)Source;
for(SIZE_T u=0; u<Size; u++)
	dst[Size-u-1]=src[u];
}
