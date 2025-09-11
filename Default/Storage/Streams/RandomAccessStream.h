//======================
// RandomAccessStream.h
//======================

#pragma once


//=======
// Using
//=======

#include "InputStream.h"
#include "OutputStream.h"


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