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

class InvalidContextException: public Exception
{
public:
	// Con-/Destructors
	InvalidContextException(): Exception(Status::InvalidContext) {}
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
