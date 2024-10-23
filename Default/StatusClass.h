//===============
// StatusClass.h
//===============

#pragma once


//========
// Status
//========

enum class Status: INT
{
Success				=0,
Error				=-1,
// Succeeded
Pending				=Success+1,
// Failed
Aborted				=Error-1,
AccessDenied		=Aborted-1,
DeviceNotReady		=AccessDenied-1,
FileExists			=DeviceNotReady-1,
FileNotFound		=FileExists-1,
InvalidArgument		=FileNotFound-1,
NotImplemented		=InvalidArgument-1,
OutOfRange			=NotImplemented-1,
Timeout				=OutOfRange-1
};


//========
// Common
//========

inline bool Succeeded(Status Status)
{
return ((INT)Status)>=0;
}

inline bool Failed(Status Status)
{
return ((INT)Status)<0;
}
