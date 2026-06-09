//==========
// Status.h
//==========

// Copyright 2026, Sven Bieg (svenbieg@outlook.de)
// https://github.com/svenbieg/Default

#pragma once


//=======
// Using
//=======

#include "Platform.h"


//========
// Status
//========

enum class Status: unsigned int
{
// Successful
Success,
Pending,
// Failed
Error=0x80000000,
Aborted,
AccessDenied,
AlreadyExists,
BufferOverrun,
ChecksumError,
ConnectionFailed,
DeviceNotReady,
DivisionByZero,
InvalidArgument,
InvalidContext,
NotFound,
NotImplemented,
NullPointer,
OutOfMemory,
OutOfRange,
Timeout
};
