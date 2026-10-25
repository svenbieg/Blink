//==================
// ReversedVolume.h
//==================

// User-data is stored backwards on embedded OTA devices.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Reversed-Volume

#pragma once


//=======
// Using
//=======

#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Storage {


//=================
// Reversed-Volume
//=================

template <class _base_t> class ReversedVolume: public _base_t
{
public:
	// Friends
	friend Object;

	// Con-/Destructors
	template <class... _args_t> static inline Handle<Volume> Create(_args_t... Arguments)
		{
		return Object::Create<ReversedVolume>(Arguments...);
		}

	// Volume
	virtual VOID Erase(UINT Block)override
		{
		UINT reverse=m_BlockCount-Block-1;
		_base_t::Erase(reverse);
		}
	virtual UINT64 GetSize()override
		{
		return 0;
		}
	virtual VOID ReadPage(UINT Block, WORD Id, Page* Page)override
		{
		UINT reverse=m_BlockCount-Block-1;
		_base_t::ReadPage(reverse, Id, Page);
		}
	virtual VOID Write(UINT Block, WORD Page, WORD Position, VOID const* Buffer, WORD Size)override
		{
		UINT reverse=m_BlockCount-Block-1;
		_base_t::Write(reverse, Page, Position, Buffer, Size);
		}

protected:
	// Con-/Destructors
	template <class... _args_t> ReversedVolume(_args_t... Arguments):
		_base_t(Arguments...),
		m_BlockCount(_base_t::m_Size/_base_t::m_BlockSize) {}

private:
	// Common
	UINT Reverse(UINT Block)
		{
		return m_BlockCount-Block-1;
		}
	UINT m_BlockCount;
};

}