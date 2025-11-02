//==================
// StatusHelper.cpp
//==================

#include "pch.h"


//=======
// Using
//=======

#include "StatusHelper.h"


//========
// Common
//========

VOID StatusHelper::ThrowIfFailed(Status status)
{
if(Failed(status))
	throw Exception(status);
}
