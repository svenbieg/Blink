//============
// Seekable.h
//============

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Storage {


//==========
// Seekable
//==========

class Seekable: public Streams::RandomAccessStream
{
public:
	// Common
	virtual FILE_SIZE GetSize()=0;
	virtual BOOL Seek(FILE_SIZE Position)=0;
};

}