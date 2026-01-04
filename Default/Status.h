//==========
// Status.h
//==========

#pragma once


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
