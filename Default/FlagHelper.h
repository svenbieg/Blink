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
	// Common
	template <class _flags_t, class _flag_t>
	static inline VOID Clear(_flags_t& Flags, _flag_t Flag)noexcept
		{
		UINT& flags=(UINT&)Flags;
		UINT flag=(UINT)Flag;
		flags&=~flag;
		}
	template <class _flags_t, class _flag_t>
	static inline BOOL Get(_flags_t Flags, _flag_t Flag)noexcept
		{
		UINT flags=(UINT)Flags;
		UINT flag=(UINT)Flag;
		return (flags&flag)>0;
		}
	template <class _flags_t, class _flag_t>
	static inline VOID Set(_flags_t& Flags, _flag_t Flag)noexcept
		{
		UINT& flags=(UINT&)Flags;
		UINT flag=(UINT)Flag;
		flags|=flag;
		}
	template <class _flags_t, class _flag_t>
	static inline VOID Set(_flags_t& Flags, _flag_t Flag, BOOL Set)noexcept
		{
		Set? FlagHelper::Set(Flags, Flag): FlagHelper::Clear(Flags, Flag);
		}
};
