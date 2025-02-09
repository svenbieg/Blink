//=============
// Exception.h
//=============

#pragma once


//=======
// Using
//=======

#include "Status.h"


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
	// Con-/Destructors
	AbortException(): Exception(Status::Aborted) {}
};

class AccessDeniedException: public Exception
{
public:
	// Con-/Destructors
	AccessDeniedException(): Exception(Status::AccessDenied) {}
};

class ConnectionFailedException: public Exception
{
public:
	// Con-/Destructors
	ConnectionFailedException(): Exception(Status::ConnectionFailed) {}
};

class DeviceNotReadyException: public Exception
{
public:
	// Con-/Destructors
	DeviceNotReadyException(): Exception(Status::DeviceNotReady) {}
};

class DivisionByZeroException: public Exception
{
public:
	// Con-/Destructors
	DivisionByZeroException(): Exception(Status::DivisionByZero) {}
};

class InvalidArgumentException: public Exception
{
public:
	// Con-/Destructors
	InvalidArgumentException(): Exception(Status::InvalidArgument) {}
};

class InvalidContextException: public Exception
{
public:
	// Con-/Destructors
	InvalidContextException(): Exception(Status::InvalidContext) {}
};

class NotFoundException: public Exception
{
public:
	// Con-/Destructors
	NotFoundException(): Exception(Status::NotFound) {}
};

class NotImplementedException: public Exception
{
public:
	// Con-/Destructors
	NotImplementedException(): Exception(Status::NotImplemented) {}
};

class OutOfMemoryException: public Exception
{
public:
	// Con-/Destructors
	OutOfMemoryException(): Exception(Status::OutOfMemory) {}
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
