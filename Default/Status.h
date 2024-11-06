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
DeviceNotReady,
FileExists,
FileNotFound,
InvalidArgument,
NotImplemented,
OutOfMemory,
OutOfRange,
Timeout
};


//========
// Common
//========

inline bool Succeeded(Status Status)
{
return !GetFlag(Status, Status::Error);
}

inline bool Failed(Status Status)
{
return GetFlag(Status, Status::Error);
}
