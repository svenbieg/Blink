//==========
// Status.h
//==========

#pragma once


//=======
// Using
//=======

#include "TypeHelper.h"


//========
// Status
//========

enum class Status: UINT
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
