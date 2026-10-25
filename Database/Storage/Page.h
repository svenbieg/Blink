//========
// Page.h
//========

// Smallest readable unit in flash-memory.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Page

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"
#include "Handle.h"


//===========
// Namespace
//===========

namespace Storage {


//======================
// Forward-Declarations
//======================

class Block;
class ErrorCorrection;
class SkipBits;
class Volume;


//======
// Page
//======

class Page: public Object, public Streams::RandomAccessStream
{
public:
	// Friends
	friend Block;
	friend ErrorCorrection;
	friend Object;
	friend SkipBits;

	// Con-/Destructors
	static Handle<Page> Create(Volume* Volume);

	// Common
	inline BYTE* Begin()const { return m_Buffer; }
	inline WORD GetPosition()const { return m_Position; }
	inline WORD GetSize()const { return m_Size; }

	// Input-Stream
	virtual SIZE_T Available()override;
	virtual SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	virtual VOID Flush()override;
	virtual SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

protected:
	// Con-/Destructors
	Page(BYTE* Buffer, SIZE_T Size, Volume* Volume);

	// Common
	BYTE* m_Buffer;
	WORD m_Position;
	WORD m_Size;
};

}