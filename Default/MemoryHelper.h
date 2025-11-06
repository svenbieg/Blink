//================
// MemoryHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===============
// Memory-Helper
//===============

class MemoryHelper
{
public:
	// Common
	static INT Compare(VOID const* Buffer1, VOID const* Buffer2, SIZE_T Size);
	template <class _value_t> static INT CompareT(_value_t const* Values1, _value_t const* Values2, SIZE_T Count)
		{
		for(SIZE_T u=0; u<Count; u++)
			{
			if(Values1[u]>Values2[u])
				return 1;
			if(Values1[u]<Values2[u])
				return -1;
			}
		return 0;
		}
	static VOID Copy(VOID* Destination, VOID const* Source, SIZE_T Size);
	template <class _value_t> static VOID CopyT(_value_t* Destination, _value_t const* Source, SIZE_T Count)
		{
		while(Count--)
			*Destination++=*Source++;
		}
	template <class _value_t> static VOID CopyT(_value_t* Destination, _value_t const* Source, _value_t* End)
		{
		while(Destination<End)
			*Destination++=*Source++;
		}
	static VOID Fill(VOID* Destination, SIZE_T Size, BYTE Value);
	template <class _value_t> static VOID FillT(_value_t* Destination, SIZE_T Count, _value_t Value)
		{
		while(Count--)
			*Destination++=Value;
		}
	template <class _value_t> static VOID FillT(_value_t* Destination, _value_t* End, _value_t Value)
		{
		while(Destination<End)
			*Destination++=Value;
		}
	static VOID Move(VOID* Destination, VOID const* Source, SIZE_T Size);
	template <class _value_t> static VOID MoveT(_value_t* Destination, _value_t const* Source, SIZE_T Count)
		{
		auto dst=Destination;
		if(Destination==Source)
			return;
		if(Destination>Source)
			{
			auto end=Destination;
			Destination+=Count;
			Source+=Count;
			while(Destination>=end)
				*--Destination=*--Source;
			}
		else
			{
			auto end=Destination+Count;
			while(Destination<end)
				*Destination++=*Source++;
			}
		}
	static inline VOID Zero(VOID* Destination, SIZE_T Size) { Fill(Destination, Size, 0); }
	template <class _value_t> static inline VOID ZeroT(_value_t* Destination, SIZE_T Count) { FillT<_value_t>(Destination, Count, _value_t(0)); }
	template <class _value_t> static inline VOID ZeroT(_value_t* Destination, _value_t* End) { FillT<_value_t>(Destination, End, _value_t(0)); }
};
