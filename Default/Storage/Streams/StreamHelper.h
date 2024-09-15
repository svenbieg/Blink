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


//========
// Common
//========

SIZE_T StreamCopy(OutputStream* Destination, InputStream* Source, SIZE_T Size=0);

}}