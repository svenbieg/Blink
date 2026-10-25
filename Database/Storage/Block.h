//=========
// Block.h
//=========

// Smallest erasable unit in flash-memory.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Block

#pragma once


//=======
// Using
//=======

#include "Storage/ErrorCorrection.h"
#include "Storage/Page.h"
#include "Storage/SkipBits.h"
#include "Storage/Volume.h"


//===========
// Namespace
//===========

namespace Storage {


//============
// Block-Mode
//============

enum class BlockMode: UINT
{
Entry=3,
File=0
};


//=======
// Block
//=======

class Block: public Object, public Streams::RandomAccessStream
{
public:
	// Friends
	friend Object;

	// Con-/Destructors
	static inline Handle<Block> Create(Volume* Volume, UINT Id, BlockMode Mode=BlockMode::Entry)
		{
		return Object::Create<Block>(Volume, Id, Mode);
		}

	// Common
	VOID Erase();
	inline Handle<Page> GetCurrentPage()const { return m_Page; }
	inline WORD GetPageCount()const { return m_PageCount; }
	inline UINT GetPosition()const { return m_Position; }
	inline UINT GetSize()const { return m_Size; }
	VOID Skip();

	// Input-Stream
	SIZE_T Available()override;
	SIZE_T Read(VOID* Buffer, SIZE_T Size)override;

	// Output-Stream
	VOID Flush()override;
	SIZE_T Write(VOID const* Buffer, SIZE_T Size)override;

private:
	// Flags
	enum class BlockFlags: UINT
		{
		None=0,
		ErrorCorrection=1,
		SkipBits=2,
		SkipPage=4
		};
	static_assert((UINT)BlockMode::File==(UINT)0);
	static_assert((UINT)BlockMode::Entry==(UINT)BlockFlags::ErrorCorrection|(UINT)BlockFlags::SkipBits);

	// Con-/Destructors
	Block(Volume* Volume, UINT Id, BlockMode Mode);

	// Common
	WORD m_Available;
	ErrorCorrection m_ErrorCorrection;
	BlockFlags m_Flags;
	UINT m_Id;
	Handle<Page> m_Page;
	WORD m_PageCount;
	WORD m_PageId;
	WORD m_PageSize;
	UINT m_Position;
	UINT m_Size;
	SkipBits m_SkipBlock;
	SkipBits m_SkipPage;
	Handle<Volume> m_Volume;
	WORD m_Writable;
};

}