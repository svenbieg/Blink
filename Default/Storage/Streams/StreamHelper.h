//================
// StreamHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "Storage/Streams/Stream.h"


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
	static SIZE_T Copy(IOutputStream* Destination, IInputStream* Source, SIZE_T Size=0);
};

}}