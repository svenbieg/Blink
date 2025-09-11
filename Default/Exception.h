//=============
// Exception.h
//=============

#pragma once


//=======
// Using
//=======

#include "StatusHelper.h"


//===========
// Exception
//===========

class Exception
{
public:
	// Con-/Destructors
	Exception(Status Status=Status::Error): m_Status(Status) {}

	// Common
	Status GetStatus()const { return m_Status; }

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
	AbortException(): Exception(Status::Aborted) {}
};

class AccessDeniedException: public Exception
{
public:
	AccessDeniedException(): Exception(Status::AccessDenied) {}
};

class BufferOverrunException: public Exception
{
public:
	BufferOverrunException(): Exception(Status::BufferOverrun) {}
};

class ConnectionFailedException: public Exception
{
public:
	ConnectionFailedException(): Exception(Status::ConnectionFailed) {}
};

class DeviceNotReadyException: public Exception
{
public:
	DeviceNotReadyException(): Exception(Status::DeviceNotReady) {}
};

class DivisionByZeroException: public Exception
{
public:
	DivisionByZeroException(): Exception(Status::DivisionByZero) {}
};

class InvalidArgumentException: public Exception
{
public:
	InvalidArgumentException(): Exception(Status::InvalidArgument) {}
};

class InvalidContextException: public Exception
{
public:
	InvalidContextException(): Exception(Status::InvalidContext) {}
};

class NotFoundException: public Exception
{
public:
	NotFoundException(): Exception(Status::NotFound) {}
};

class NotImplementedException: public Exception
{
public:
	NotImplementedException(): Exception(Status::NotImplemented) {}
};

class OutOfMemoryException: public Exception
{
public:
	OutOfMemoryException(): Exception(Status::OutOfMemory) {}
};

class OutOfRangeException: public Exception
{
public:
	OutOfRangeException(): Exception(Status::OutOfRange) {}
};

class TimeoutException: public Exception
{
public:
	TimeoutException(): Exception(Status::Timeout) {}
};
