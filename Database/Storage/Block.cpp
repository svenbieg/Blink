//===========
// Block.cpp
//===========

// Smallest erasable unit in flash-memory.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Block

#include "Block.h"


//=======
// Using
//=======

#include "Storage/Volume.h"
#include "FlagHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//========
// Common
//========

VOID Block::Erase()
{
m_Volume->Erase(m_Id);
m_Available=0;
m_Page=nullptr;
m_PageId=-1;
m_Position=0;
m_Writable=0;
}

VOID Block::Skip()
{
assert(FlagHelper::Get(m_Flags, BlockFlags::SkipBits));
FlagHelper::Set(m_Flags, BlockFlags::SkipPage);
if(m_PageId==-1)
	{
	m_Page=m_Volume->ReadPage(m_Id, 0);
	m_PageId=0;
	WORD skip_min=0;
	skip_min+=m_SkipBlock.ReadFromPage(m_Page);
	skip_min+=m_SkipPage.ReadFromPage(m_Page);
	if(m_SkipPage.m_SkipCount==0)
		m_SkipPage.m_SkipCount=skip_min/ErrorCorrection::BLOCK_SIZE;
	}
WORD page_id=m_SkipBlock.m_SkipCount;
if(m_PageId!=page_id)
	{
	m_Page=m_Volume->ReadPage(m_Id, page_id);
	m_PageId=page_id;
	m_SkipPage.ReadFromPage(m_Page);
	m_Available=0;
	}
WORD page_pos=m_SkipPage.m_SkipCount*ErrorCorrection::BLOCK_SIZE;
if(m_Page->m_Position!=page_pos)
	{
	m_Page->m_Position=page_pos;
	m_Available=0;
	}
m_Position=m_PageId*m_PageSize+page_pos;
}


//==============
// Input-Stream
//==============

SIZE_T Block::Available()
{
return m_Size-m_Position;
}

SIZE_T Block::Read(VOID* buf, SIZE_T size)
{
auto dst=(BYTE*)buf;
SIZE_T read=0;
while(read<size)
	{
	WORD page_pos=m_Position%m_PageSize;
	if(page_pos==0)
		{
		WORD page_id=m_Position/m_PageSize;
		if(page_id>=m_PageCount)
			throw OutOfRangeException();
		if(m_PageId!=page_id)
			{
			m_Page=m_Volume->ReadPage(m_Id, page_id);
			m_PageId=page_id;
			}
		if(FlagHelper::Get(m_Flags, BlockFlags::SkipBits))
			{
			WORD skip_min=0;
			if(m_PageId==0)
				skip_min+=m_SkipBlock.ReadFromPage(m_Page);
			skip_min+=m_SkipPage.ReadFromPage(m_Page);
			if(m_SkipPage.m_SkipCount==0)
				m_SkipPage.m_SkipCount=skip_min/ErrorCorrection::BLOCK_SIZE;
			if(FlagHelper::Get(m_Flags, BlockFlags::SkipPage))
				{
				page_pos=m_SkipPage.m_SkipCount*ErrorCorrection::BLOCK_SIZE;
				m_Page->m_Position=page_pos;
				}
			m_Position+=page_pos;
			}
		}
	if(FlagHelper::Get(m_Flags, BlockFlags::ErrorCorrection))
		{
		if(!m_Available)
			{
			m_Available=m_ErrorCorrection.Available(m_Page);
			if(!m_Available)
				{
				m_Position=(m_PageId+1)*m_PageSize;
				continue;
				}
			}
		WORD copy=TypeHelper::Min(m_Available, size-read);
		read+=m_Page->Read(&dst[read], copy);
		m_Available-=copy;
		m_Position=m_PageId*m_PageSize+m_Page->m_Position;
		}
	else
		{
		WORD available=m_Page->Available();
		WORD copy=TypeHelper::Min(available, size-read);
		read+=m_Page->Read(&dst[read], copy);
		m_Position+=read;
		}
	}
return read;
}


//===============
// Output-Stream
//===============

VOID Block::Flush()
{
if(m_PageId==-1)
	return;
if(FlagHelper::Get(m_Flags, BlockFlags::ErrorCorrection))
	{
	m_ErrorCorrection.Flush(m_Page);
	m_Writable=0;
	}
auto buf=m_Page->Begin();
m_Volume->Write(m_Id, m_PageId, 0, buf, m_PageSize);
}

SIZE_T Block::Write(VOID const* buf, SIZE_T size)
{
auto src=(BYTE const*)buf;
SIZE_T written=0;
while(written<size)
	{
	WORD page_pos=m_Position%m_PageSize;
	if(page_pos==0)
		{
		if(m_PageId!=-1)
			Flush();
		WORD page_id=m_Position/m_PageSize;
		if(page_id>=m_PageCount)
			throw OutOfRangeException();
		m_Page=Page::Create(m_Volume);
		m_PageId=page_id;
		if(FlagHelper::Get(m_Flags, BlockFlags::SkipBits))
			{
			WORD skip_min=0;
			if(m_PageId==0)
				{
				m_SkipBlock.m_SkipCount=0;
				skip_min+=m_SkipBlock.WriteToStream(m_Page);
				}
			skip_min+=m_SkipPage.GetSize();
			m_SkipPage.m_SkipCount=skip_min/ErrorCorrection::BLOCK_SIZE;
			m_SkipPage.WriteToStream(m_Page);
			page_pos=skip_min;
			m_Position+=page_pos;
			}
		m_Writable=0;
		}
	if(FlagHelper::Get(m_Flags, BlockFlags::ErrorCorrection))
		{
		if(!m_Writable)
			{
			m_Writable=m_ErrorCorrection.Writable(m_Page);
			if(!m_Writable)
				{
				m_Position=(m_PageId+1)*m_PageSize;
				continue;
				}
			}
		WORD copy=TypeHelper::Min(m_Writable, size-written);
		written+=m_Page->Write(&src[written], copy);
		m_Writable-=copy;
		if(m_Writable==0)
			m_ErrorCorrection.Flush(m_Page);
		m_Position=m_PageId*m_PageSize+m_Page->m_Position;
		}
	else
		{
		WORD available=m_Page->Available();
		WORD copy=TypeHelper::Min(available, size-written);
		written+=m_Page->Write(&src[written], copy);
		m_Position+=written;
		}
	}
return size;
}


//==========================
// Con-/Destructors Private
//==========================

Block::Block(Volume* volume, UINT id, BlockMode mode):
m_Available(0),
m_Flags((BlockFlags)mode),
m_Id(id),
m_PageCount(volume->GetBlockSize()/volume->GetPageSize()),
m_PageId(-1),
m_PageSize(volume->GetPageSize()),
m_Position(0),
m_Size(volume->GetBlockSize()),
m_SkipBlock(m_PageCount),
m_SkipPage(m_PageSize/ErrorCorrection::BLOCK_SIZE),
m_Volume(volume),
m_Writable(0)
{}

}