//===============
// ErrorHelper.h
//===============

#pragma once


//=======
// Using
//=======

#include <assert.h>


//========
// Common
//========

[[noreturn]] VOID Abort();
[[noreturn]] VOID Abort(LPCSTR File, UINT Line, LPCSTR Function, LPCSTR Expression);

#ifdef _DEBUG

VOID DebugPrint(Handle<String> Message);

template <class... _args_t> inline VOID DebugPrint(LPCSTR Format, _args_t... Arguments)
{
Handle<String> msg=new String(Format, Arguments...);
DebugPrint(msg);
}

#else

#define DebugPrint(...)

#endif


//===========
// Exception
//===========

class Exception
{
public:
	// Common
	Status GetStatus()const { return m_Status; }

protected:
	// Con-/Destructors
	Exception(Status Status): m_Status(Status) {}

private:
	// Common
	Status m_Status;
};


//============
// Exceptions
//============

class AbortException: public Exception
{
public:
	// Con-/Destructors
	AbortException(): Exception(Status::Aborted) {}
};

class DeviceNotReadyException: public Exception
{
public:
	// Con-/Destructors
	DeviceNotReadyException(): Exception(Status::DeviceNotReady) {}
};

class InvalidArgumentException: public Exception
{
public:
	// Con-/Destructors
	InvalidArgumentException(): Exception(Status::InvalidArgument) {}
};

class NotImplementedException: public Exception
{
public:
	// Con-/Destructors
	NotImplementedException(): Exception(Status::NotImplemented) {}
};

class OutOfRangeException: public Exception
{
public:
	// Con-/Destructors
	OutOfRangeException(): Exception(Status::OutOfRange) {}
};

class TimeoutException: public Exception
{
public:
	// Con-/Destructors
	TimeoutException(): Exception(Status::Timeout) {}
};
