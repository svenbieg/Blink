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
	static VOID Copy(VOID* To, VOID const* From, SIZE_T Size);
	template <class _value_t> static VOID CopyT(_value_t* To, _value_t const* From, SIZE_T Count)
		{
		while(Count--)
			*To++=*From++;
		}
	template <class _value_t> static VOID CopyT(_value_t* To, _value_t const* From, _value_t* End)
		{
		while(To<End)
			*To++=*From++;
		}
	static VOID Fill(VOID* To, SIZE_T Size, BYTE Value);
	template <class _value_t> static VOID FillT(_value_t* To, SIZE_T Count, _value_t Value)
		{
		while(Count--)
			*To++=Value;
		}
	template <class _value_t> static VOID FillT(_value_t* To, _value_t* End, _value_t Value)
		{
		while(To<End)
			*To++=Value;
		}
	static VOID Move(VOID* To, VOID const* From, SIZE_T Size);
	template <class _value_t> static VOID MoveT(_value_t* To, _value_t const* From, SIZE_T Count)
		{
		auto dst=To;
		if(To==From)
			return;
		if(To>From)
			{
			auto end=To;
			To+=Count;
			From+=Count;
			while(To>=end)
				*--To=*--From;
			}
		else
			{
			auto end=To+Count;
			while(To<end)
				*To++=*From++;
			}
		}
	static inline VOID Zero(VOID* To, SIZE_T Size) { Fill(To, Size, 0); }
	template <class _value_t> static inline VOID ZeroT(_value_t* To, SIZE_T Count) { FillT<_value_t>(To, Count, _value_t(0)); }
	template <class _value_t> static inline VOID ZeroT(_value_t* To, _value_t* End) { FillT<_value_t>(To, End, _value_t(0)); }
};
