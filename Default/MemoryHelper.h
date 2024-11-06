//================
// MemoryHelper.h
//================

#pragma once


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
