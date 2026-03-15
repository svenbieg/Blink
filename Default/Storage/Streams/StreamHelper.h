//================
// StreamHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/InputStream.h"
#include "Storage/Streams/OutputStream.h"


//===========
// Namespace
//===========

namespace Storage {
	namespace Streams {


//===============
// Stream-Helper
//===============

class StreamHelper
{
public:
	// Common
	static SIZE_T Copy(OutputStream* Destination, InputStream* Source, SIZE_T Size=0);
};

}}