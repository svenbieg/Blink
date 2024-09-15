//================
// OutputStream.h
//================

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


//===============
// Output-Stream
//===============

class OutputStream: public virtual Stream
{
public:
	// Common
	virtual VOID Flush()=0;
	virtual SIZE_T Write(VOID const* Buffer, SIZE_T Size)=0;
};

}}