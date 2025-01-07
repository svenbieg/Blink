//==========
// Status.h
//==========

#pragma once


//=======
// Using
//=======

#include "FlagHelper.h"


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
ConnectionFailed,
DeviceNotReady,
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


//========
// Common
//========

inline BOOL Succeeded(Status Status)
{
return !FlagHelper::Get(Status, Status::Error);
}

inline BOOL Failed(Status Status)
{
return FlagHelper::Get(Status, Status::Error);
}
