//===================
// ErrorCorrection.h
//===================

// Checksum computation for variable sized blocks.

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Database/wiki/Storage#Error-Correction

#pragma once


//=======
// Using
//=======

#include "Storage/Page.h"


//===========
// Namespace
//===========

namespace Storage {


//==================
// Error-Correction
//==================

class ErrorCorrection
{
public:
	// Friends
	friend Block;

private:
	// Settings
	static const UINT BLOCK_SIZE=8;

	// Con-/Destructors
	ErrorCorrection();

	// Common
	WORD Available(Page* Page);
	VOID ChecksumX(BYTE* Checksum, WORD Size, BYTE const* Buffer);
	VOID ChecksumY(BYTE* Checksum, WORD Size, BYTE const* Buffer);
	VOID Correct(BYTE* Buffer, WORD Size);
	VOID CorrectX(BYTE const* ErrorY, BYTE Y, WORD Size, BYTE* Buffer);
	VOID CorrectY(BYTE const* ErrorX, BYTE X, WORD Size, BYTE* Buffer);
	BYTE ErrorX(BYTE* Error, WORD Size, BYTE const* Buffer, BYTE* Y);
	BYTE ErrorY(BYTE* Error, WORD Size, BYTE const* Buffer, BYTE* X);
	VOID Flush(Page* Page);
	WORD GetSize(Page*, WORD Bits);
	BOOL GetSize(BYTE Bits, WORD* Size);
	WORD Writable(Page* Page);
	WORD m_Position;
	WORD m_Size;
};

}