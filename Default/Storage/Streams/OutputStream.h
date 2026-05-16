//==========
// Stream.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// Output-Stream
//===============

class OutputStream
{
public:
	// Common
	virtual VOID Flush()=0;
	virtual SIZE_T Write(VOID const* Buffer, SIZE_T Size)=0;
	static inline SIZE_T Write(OutputStream* Stream, VOID const* Buffer, SIZE_T Size)
		{
		if(!Stream)
			return Size;
		return Stream->Write(Buffer, Size);
		}
};

}}