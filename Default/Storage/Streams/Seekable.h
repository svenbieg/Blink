//==========
// Stream.h
//==========

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/RandomAccessStream.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==========
// Seekable
//==========

class Seekable: public RandomAccessStream
{
public:
	// Common
	virtual SIZE_T GetSize()=0;
	virtual BOOL Seek(SIZE_T Position)=0;
};

}}