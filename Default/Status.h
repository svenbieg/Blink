//==========
// Status.h
//==========

#pragma once


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
BufferOverrun,
ConnectionFailed,
DeviceNotReady,
DivisionByZero,
FileExists,
FileNotFound,
InvalidArgument,
InvalidContext,
NotFound,
NotImplemented,
OutOfMemory,
OutOfRange,
Timeout
};
