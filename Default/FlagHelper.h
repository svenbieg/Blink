//==============
// FlagHelper.h
//==============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//=============
// Flag-Helper
//=============

class FlagHelper
{
public:
	// Common
	template <class _flags_t, class _flag_t> static inline BOOL Clear(_flags_t& Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<UINT>(Flags);
		auto flag=static_cast<UINT>(Flag);
		BOOL old_flag=(flags&flag)!=0;
		flags&=~flag;
		Flags=(_flags_t)flags;
		return old_flag;
		}
	template <class _flags_t, class _flag_t> static inline BOOL Get(_flags_t Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<UINT>(Flags);
		auto flag=static_cast<UINT>(Flag);
		return (flags&flag)!=0;
		}
	template <class _flags_t, class _flag_t> static inline VOID Set(_flags_t& Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<UINT>(Flags);
		auto flag=static_cast<UINT>(Flag);
		flags|=flag;
		Flags=(_flags_t)flags;
		}
	template <class _flags_t, class _flag_t> static inline VOID Set(_flags_t& Flags, _flag_t Flag, BOOL Set)noexcept
		{
		if(Set)
			{
			FlagHelper::Set(Flags, Flag);
			}
		else
			{
			FlagHelper::Clear(Flags, Flag);
			}
		}
	template <class _flags_t, class _flag_t> static inline BOOL Switch(_flags_t& Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<UINT>(Flags);
		auto flag=static_cast<UINT>(Flag);
		flags^=flag;
		Flags=(_flags_t)flags;
		return (flags&flag)!=0;
		}
};
