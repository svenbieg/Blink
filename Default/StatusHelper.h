//================
// StatusHelper.h
//================

#pragma once


//=======
// Using
//=======

#include "FlagHelper.h"
#include "Status.h"


//===============
// Status-Helper
//===============

class StatusHelper
{
public:
	// Common
	static inline BOOL Failed(Status Status)
		{
		return FlagHelper::Get(Status, Status::Error);
		}
	static inline BOOL Succeeded(Status Status)
		{
		return !FlagHelper::Get(Status, Status::Error);
		}
	static inline VOID ThrowIfFailed(Status Status)
		{
		if(Failed(Status))
			throw Status;
		}
};
