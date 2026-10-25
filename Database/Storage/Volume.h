//==========
// Volume.h
//==========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage

#pragma once


//=======
// Using
//=======

#include "Storage/Page.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Volume
//========

class Volume: public virtual Object
{
public:
	// Using
	using Page=Storage::Page;

	// Common
	virtual VOID Erase(UINT Block)=0;
	virtual UINT GetBlockSize()=0;
	virtual WORD GetPageSize(WORD* Spare=nullptr)=0;
	virtual UINT64 GetSize()=0;
	virtual Handle<Page> ReadPage(UINT Block, WORD Page)=0;
	virtual VOID Write(UINT Block, WORD Page, WORD Position, VOID const* Buffer, WORD Size)=0;

protected:
	// Con-/Destructors
	Volume()=default;
};

}