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
if(Failed(status))
	throw Exception(status);
}
