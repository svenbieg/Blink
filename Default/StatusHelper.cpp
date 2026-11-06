//==================
// StatusHelper.cpp
//==================

#include "StatusHelper.h"


//=======
// Using
//=======

#include "Exception.h"


//========
// Common
//========

VOID StatusHelper::ThrowIfFailed(Status status)
{
if(Succeeded(status))
	return;
switch(status)
	{
	case Status::Aborted:
		throw AbortException();
	case Status::AccessDenied:
		throw AccessDeniedException();
	case Status::AlreadyExists:
		throw AlreadyExistsException();
	case Status::BufferOverrun:
		throw BufferOverrunException();
	case Status::ChecksumError:
		throw ChecksumException();
	case Status::ConnectionFailed:
		throw ConnectionFailedException();
	case Status::DeviceNotReady:
		throw DeviceNotReadyException();
	case Status::DivisionByZero:
		throw DivisionByZeroException();
	case Status::InvalidArgument:
		throw InvalidArgumentException();
	case Status::InvalidContext:
		throw InvalidContextException();
	case Status::NotFound:
		throw NotFoundException();
	case Status::NotImplemented:
		throw NotImplementedException();
	case Status::NullPointer:
		throw NullPointerException();
	case Status::OutOfMemory:
		throw OutOfMemoryException();
	case Status::OutOfRange:
		throw OutOfRangeException();
	case Status::Timeout:
		throw TimeoutException();
	default:
		{
		assert(false);
		throw Exception(status);
		}
	}
}