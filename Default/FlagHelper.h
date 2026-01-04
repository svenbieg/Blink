//==============
// FlagHelper.h
//==============

#pragma once


//========
// Common
//========

class FlagHelper
{
public:
	// Using
	using uint32_t=unsigned int;

	// Common
	template <class _flags_t, class _flag_t> static inline void Clear(_flags_t& Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<uint32_t>(Flags);
		auto flag=static_cast<uint32_t>(Flag);
		flags&=~flag;
		Flags=(_flags_t)flags;
		}
	template <class _flags_t, class _flag_t> static inline bool Get(_flags_t Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<uint32_t>(Flags);
		auto flag=static_cast<uint32_t>(Flag);
		return (flags&flag)>0;
		}
	template <class _flags_t, class _flag_t> static inline void Set(_flags_t& Flags, _flag_t Flag)noexcept
		{
		static_assert(sizeof(_flags_t)<=4);
		static_assert(sizeof(_flag_t)<=4);
		auto flags=static_cast<uint32_t>(Flags);
		auto flag=static_cast<uint32_t>(Flag);
		flags|=flag;
		Flags=(_flags_t)flags;
		}
	template <class _flags_t, class _flag_t> static inline void Set(_flags_t& Flags, _flag_t Flag, bool Set)noexcept
		{
		Set? FlagHelper::Set(Flags, Flag): FlagHelper::Clear(Flags, Flag);
		}
};
