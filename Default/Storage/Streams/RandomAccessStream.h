//======================
// RandomAccessStream.h
//======================

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


//======================
// Random-Access-Stream
//======================

class RandomAccessStream: public InputStream, public OutputStream
{};

}}