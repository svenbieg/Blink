//==============
// SkipBits.cpp
//==============

// Skipping pages and chunks out of date.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Skip-Bits

#include "SkipBits.h"


//=======
// Using
//=======

#include "Storage/Block.h"
#include "BitHelper.h"


//===========
// Namespace
//===========

namespace Storage {


//============================
// Con-/Destructors Protected
//============================

SkipBits::SkipBits(WORD bits_count):
m_BitsCount(bits_count/32),
m_SkipCount(0)
{}


//==================
// Common Protected
//==================

WORD SkipBits::ReadFromPage(Page* page)
{
m_SkipCount=0;
WORD page_pos=page->m_Position;
auto buf=page->Begin();
WORD pos=page_pos;
for(UINT u=0; u<m_BitsCount; u++)
	{
	UINT bits=0;
	MemoryHelper::Copy(&bits, &buf[pos], sizeof(UINT));
	UINT skip=BitHelper::CountTrailingZeros(bits);
	if(!skip)
		break;
	m_SkipCount+=skip;
	pos+=sizeof(UINT);
	}
WORD size=m_SkipCount*sizeof(UINT);
page->m_Position=page_pos+size;
return size;
}

WORD SkipBits::WriteToStream(OutputStream* stream)
{
WORD size=m_BitsCount*sizeof(UINT);
if(!stream)
	return size;
for(WORD u=0; u<m_BitsCount; u++)
	{
	UINT bits=GetBits(u*32, m_SkipCount);
	stream->Write(&bits, sizeof(UINT));
	}
return size;
}


//================
// Common Private
//================

UINT SkipBits::GetBits(WORD pos, WORD skip)
{
if(pos>=skip)
	return -1;
UINT bits=-1;
UINT shift=TypeHelper::Min(skip-pos, 32);
bits>>=shift;
bits<<=shift;
return bits;
}

}