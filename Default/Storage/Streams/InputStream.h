//===============
// InputStream.h
//===============

#pragma once


//=======
// Using
//=======

#include "Stream.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//==============
// Input-Stream
//==============

class InputStream: public virtual Stream
{
public:
	// Common
	virtual SIZE_T Available()=0;
	virtual SIZE_T Read(VOID* Buffer, SIZE_T Size)=0;
};

}}