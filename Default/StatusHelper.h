//================
// StatusHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "Exception.h"
#include "FlagHelper.h"
#include "Status.h"


//===============
// Status-Helper
//===============

class StatusHelper
{
public:
	// Common
	static inline bool Failed(Status Status)
		{
		return FlagHelper::Get(Status, Status::Error);
		}
	static inline bool Succeeded(Status Status)
		{
		return !FlagHelper::Get(Status, Status::Error);
		}
	static void ThrowIfFailed(Status Status)
		{
		if(Failed(Status))
			throw Exception(Status);
		}
};
