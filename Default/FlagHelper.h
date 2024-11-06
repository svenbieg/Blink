//==============
// FlagHelper.h
//==============

#pragma once


//========
// Common
//========

#undef ClearFlag
#undef SetFlag

template <class _flags_t, class _flag_t>
inline void ClearFlag(_flags_t& Flags, _flag_t Flag)noexcept
{
UINT& flags=(UINT&)Flags;
UINT flag=(UINT)Flag;
flags&=~flag;
}

template <class _flags_t, class _flag_t>
inline bool GetFlag(_flags_t Flags, _flag_t Flag)noexcept
{
UINT flags=(UINT)Flags;
UINT flag=(UINT)Flag;
return (flags&flag)>0;
}

template <class _flags_t, class _flag_t>
inline void SetFlag(_flags_t& Flags, _flag_t Flag)noexcept
{
UINT& flags=(UINT&)Flags;
UINT flag=(UINT)Flag;
flags|=flag;
}

template <class _flags_t, class _flag_t>
inline void SetFlag(_flags_t& Flags, _flag_t Flag, bool Set)noexcept
{
Set? SetFlag(Flags, Flag): ClearFlag(Flags, Flag);
}
