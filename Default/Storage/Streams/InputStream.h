//===============
// InputStream.h
//===============

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

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


//==============
// Input-Stream
//==============

class InputStream
{
public:
	// Common
	virtual SIZE_T Available()=0;
	virtual SIZE_T Read(VOID* Buffer, SIZE_T Size)=0;
};

}}